#include <spdlog/spdlog.h>

#include <spdlog/sinks/basic_file_sink.h>

#include "common/NodeConfiguration.hpp"
#include "common/serialization/BlockSerializer.hpp"
#include "common/utils/Time.hpp"

#include "db/ConsensusStorage.hpp"
#include "db/RedisDB.hpp"

#include "io/ChannelStore.hpp"
#include "io/ConnectionStore.hpp"
#include "io/ConsensusChannel.hpp"
#include "io/ContributionWrapperSerializer.hpp"
#include "io/ElectionValueWrapperSerializer.hpp"
#include "io/HeaderSerializer.hpp"
#include "io/Utils.hpp"

#include "slot/SlotHandler.hpp"
#include "slot/TimerWheel.hpp"

#include "Buffer.hpp"
#include "ConfigurationReader.hpp"
#include "Consumer.hpp"

void logConfiguration(const ::common::NodeConfiguration &config) {
  const auto &nodes{config.nodes};
  spdlog::info("Loaded {} node(s)", nodes.size());
  for (const auto &node : nodes) {
    spdlog::info("name: {}, address: {}", node.name, node.address);
  }
}

int main(int argc, char **argv) {

  spdlog::set_level(spdlog::level::info);
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
  auto defaultFileLogger = spdlog::basic_logger_mt(
      "default_logger" + configValue.self.name,
      "logs/logs_" + configValue.self.name + ".txt", true);
  spdlog::set_default_logger(defaultFileLogger);

  ::db::RedisDB redis{};
  ::db::ConsensusStorage consensusStorage{configValue};
  Buffer buffer{};
  Consumer consumer(configValue, buffer);
  ::slot::TimerWheel timerWheel{};
  ::io::ConnectionStore connectionStore{};
  ::io::ChannelStore channelStore{};
  timerWheel.subscribe([&consensusStorage]() {
    const auto slot = ::common::utils::Time::getSlot();
    consensusStorage.initConditions(slot);
    consensusStorage.initContexts(slot);
  });
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

    spdlog::debug("[{}][{}][{}] Received: {} , ", localNodeInfo.address,
                  localNodeInfo.name, deserializedHeader.slot, msg.body());

    if (deserializedHeader.operation == ::io::ConsensusOperation::INSPECTION) {
      ::io::ContributionWrapperSerializer contributionWrapperSerializer{};
      const auto &contributionWrapper =
          contributionWrapperSerializer.deserialize(body);
      consensusStorage.initConditions(deserializedHeader.slot);
      consensusStorage.addContext(deserializedHeader.address,
                                  deserializedHeader.node,
                                  deserializedHeader.slot);
      consensusStorage.setContribution(deserializedHeader.address,
                                       deserializedHeader.slot,
                                       contributionWrapper.isContributing);

      if (consensusStorage.areAllContributorsConfirmed(
              deserializedHeader.slot)) {
        consensusStorage.getSlotSynchronizationContext(deserializedHeader.slot)
            ->isSynchronized.store(true);
        consensusStorage.getSlotSynchronizationContext(deserializedHeader.slot)
            ->condition.notify_all();
      }
    }

    if (deserializedHeader.operation == ::io::ConsensusOperation::ELECTION) {
      ::io::ElectionValueWrapperSerializer electionValueWrapperSerializer{};
      const auto &electionWrapper =
          electionValueWrapperSerializer.deserialize(body);
      consensusStorage.fillElectionValue(deserializedHeader.address,
                                         deserializedHeader.slot,
                                         electionWrapper.value);

      if (consensusStorage.areAllElectionValuesPresent(
              deserializedHeader.slot)) {
        consensusStorage.getSlotSynchronizationContext(deserializedHeader.slot)
            ->isSynchronized.store(true);
        consensusStorage.getSlotSynchronizationContext(deserializedHeader.slot)
            ->condition.notify_all();
      }
    }

    if (deserializedHeader.operation == ::io::ConsensusOperation::UPLOADING) {
      ::serialization::BlockSerializer blockSerializer{};
      const auto &block = blockSerializer.deserialize(body);
      consensusStorage.addBlock(deserializedHeader.address,
                                deserializedHeader.slot, block);

      if (consensusStorage.isAllContributorsDataCollected(
              deserializedHeader.slot)) {
        consensusStorage.getSlotSynchronizationContext(deserializedHeader.slot)
            ->isSynchronized.store(true);
        consensusStorage.getSlotSynchronizationContext(deserializedHeader.slot)
            ->condition.notify_all();
      }
    }

    if (deserializedHeader.operation == ::io::ConsensusOperation::BROADCAST) {
      ::serialization::BlockSerializer blockSerializer{};
      const auto &block = blockSerializer.deserialize(body);
      consensusStorage.addBlock(deserializedHeader.address,
                                deserializedHeader.slot, block);
      consensusStorage.marAsResolved(deserializedHeader.address,
                                     deserializedHeader.slot);

      consensusStorage.getSlotSynchronizationContext(deserializedHeader.slot)
          ->isSynchronized.store(true);
      consensusStorage.getSlotSynchronizationContext(deserializedHeader.slot)
          ->condition.notify_all();
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
    if (configValue.self.address == item.first) {
      continue;
    }
    std::thread t([=, &item]() {
      auto *loop = item.second.loop;
      auto *connection = item.second.connection.get();
      ev_run(loop, 0);
    });
    t.detach();
  }

  timerWheel.start();
  std::thread{[&consumer]() { consumer.run(); }}.detach();

  auto &selfConnectionData = connectionStore.get().at(configValue.self.address);
  auto *loop = selfConnectionData.loop;
  ev_run(loop, 0);
  return 0;
}
