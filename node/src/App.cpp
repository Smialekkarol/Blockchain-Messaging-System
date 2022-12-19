#include <spdlog/spdlog.h>

#include "common/NodeConfiguration.hpp"
#include "common/serialization/BlockSerializer.hpp"
#include "common/utils/Time.hpp"

#include "db/ConsensusStorage.hpp"
#include "db/RedisDB.hpp"

#include "io/ChannelStore.hpp"
#include "io/ConnectionStore.hpp"
#include "io/ConsensusChannel.hpp"
#include "io/ContributionWrapperSerializer.hpp"
#include "io/HeaderSerializer.hpp"
#include "io/Utils.hpp"

#include "slot/SlotHandler.hpp"
#include "slot/TimerWheel.hpp"

#include "Buffer.hpp"
#include "ConfigurationReader.hpp"
#include "Consumer.hpp"

void logConfiguration(const common::NodeConfiguration &config) {
  const auto &nodes{config.nodes};
  spdlog::info("Loaded {} node(s)", nodes.size());
  for (const auto &node : nodes) {
    spdlog::info("name: {}, address: {}", node.name, node.address);
  }
}

int main(int argc, char **argv) {
  spdlog::set_level(spdlog::level::debug);
  if (argc < 2) {
    spdlog::error("Usage: {}: <node_configuration_file>", argv[0]);
    return 1;
  }
  const auto &config = ConfigurationReader::read(argv[1]);
  if (not config.has_value()) {
    spdlog::error("Failed to read configuration");
    return 2;
  }
  const auto &configValue = config.value();
  logConfiguration(configValue);

  ::db::RedisDB redis{};
  ::db::ConsensusStorage consensusStorage{};
  Buffer buffer{};
  Consumer consumer(configValue, buffer);
  ::slot::TimerWheel timerWheel{};
  ::io::ConnectionStore connectionStore{};
  ::io::ChannelStore channelStore{};
  timerWheel.subscribe([&configValue, &consensusStorage, &redis, &buffer,
                        &consumer, &channelStore]() {
    std::thread t([=, &consensusStorage, &configValue, &redis, &buffer,
                   &consumer, &channelStore]() {
      ::slot::SlotHandler slotHandler{
          configValue, consensusStorage, redis, buffer, consumer, channelStore};
      slotHandler.handle();
    });
    t.detach();
  });
  timerWheel.subscribe([&redis]() {
    constexpr std::uint16_t minute{120};
    const auto slot = ::common::utils::Time::getSlot();
    if (slot % minute == 0) {
      redis.save();
    }
  });

  connectionStore.push(configValue.self);
  const std::string &localAddress = configValue.self.address;
  auto localConsensusChannel = std::make_unique<::io::ConsensusChannel>(
      connectionStore.get(localAddress));
  localConsensusChannel.get()->consume([localNodeInfo = configValue.self,
                                        &consensusStorage](
                                           const AMQP::Message &msg,
                                           uint64_t tag, bool redelivered) {
    const auto &content = ::io::split(msg.body());
    const auto &header = content.first;
    const auto &body = content.second;
    ::io::HeaderSerializer headerSerializer{};
    const auto &deserializedHeader = headerSerializer.deserialize(header);

    if (deserializedHeader.operation != ::io::ConsensusOperation::INSPECTION) {
      spdlog::error("[{}]{} Received unsupported operation: {}",
                    localNodeInfo.address, localNodeInfo.name,
                    static_cast<std::uint16_t>(deserializedHeader.operation));
      return;
    }

    if (deserializedHeader.operation == ::io::ConsensusOperation::INSPECTION) {
      ::io::ContributionWrapperSerializer contributionWrapperSerializer{};
      const auto &contributionWrapper =
          contributionWrapperSerializer.deserialize(body);
      if (contributionWrapper.isContributing) {
        spdlog::info(
            "[{}]{} Received: operation {}, node {}, address {} , timestamp "
            "{} , slot {} , isContributing {}",
            localNodeInfo.address, localNodeInfo.name,
            static_cast<std::uint16_t>(deserializedHeader.operation),
            deserializedHeader.node, deserializedHeader.address,
            deserializedHeader.timestamp, deserializedHeader.slot,
            contributionWrapper.isContributing);
      }
      consensusStorage.addContext(deserializedHeader.address,
                                  deserializedHeader.node,
                                  deserializedHeader.slot);
      consensusStorage.setContribution(deserializedHeader.address,
                                       deserializedHeader.slot,
                                       contributionWrapper.isContributing);
    }
  });

  channelStore.pushLocal(localAddress, std::move(localConsensusChannel));

  for (const auto &node : configValue.nodes) {
    connectionStore.push(node);
    const std::string &remoteAddress = node.address;
    auto remoteConsensusChannel = std::make_unique<::io::ConsensusChannel>(
        connectionStore.get(remoteAddress));
    channelStore.pushRemote(remoteAddress, std::move(remoteConsensusChannel));
  }

  for (auto &item : connectionStore.get()) {
    std::thread t([=, &item]() {
      auto *loop = item.second.loop;
      auto *connection = item.second.connection.get();
      ev_run(loop, 0);
    });
    t.detach();
  }

  timerWheel.start();
  std::thread{[&consumer]() { consumer.run(); }}.detach();

  while (true) {
  }

  return 0;
}
