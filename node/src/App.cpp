#include "Buffer.hpp"
#include "ConfigurationReader.hpp"
#include "Consumer.hpp"
#include "common/NodeConfiguration.hpp"
#include "common/utils/Time.hpp"
#include "db/RedisDB.hpp"
#include "slot/SlotHandler.hpp"
#include "slot/TimerWheel.hpp"
#include <spdlog/spdlog.h>

#include "io/ChannelStore.hpp"
#include "io/ConnectionStore.hpp"
#include "io/ConsensusChannel.hpp"

#include "common/serialization/BlockSerializer.hpp"
#include "io/HeaderSerializer.hpp"
#include "io/Utils.hpp"

#include "utils/Time.hpp"
#include <iostream>

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
  Buffer buffer{};
  Consumer consumer(configValue, buffer);
  ::slot::TimerWheel timerWheel{};
  ::slot::SlotHandler slotHandler{redis, buffer, consumer};
  timerWheel.subscribe([&redis, &buffer, &consumer]() {
    std::thread t([=, &redis, &buffer, &consumer]() {
      ::slot::SlotHandler slotHandler{redis, buffer, consumer};
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

  ::io::ConnectionStore connectionStore{};
  ::io::ChannelStore channelStore{};

  connectionStore.push(configValue.self);
  const std::string &localAddress = configValue.self.address;
  auto localConsensusChannel = std::make_unique<::io::ConsensusChannel>(
      connectionStore.get(localAddress));
  localConsensusChannel.get()->consume(
      [localNodeInfo = configValue.self](const AMQP::Message &msg, uint64_t tag,
                                         bool redelivered) {
        spdlog::info("[{}]{} Received: ", localNodeInfo.address,
                     localNodeInfo.name);
        spdlog::info("msg.body(): {}", msg.body());
        std::cout << "msgbody from cout: " << msg.body() << std::endl;
        const auto &content = ::io::split(msg.body());
        const auto &header = content.first;
        const auto &body = content.second;
        // spdlog::info("header: {}", header);
        // spdlog::info("body: {}", body);

        ::serialization::BlockSerializer blockSerializer{};
        ::io::HeaderSerializer headerSerializer{};
        const auto deserializedHeader = headerSerializer.deserialize(header);
        const auto deserializedBody = blockSerializer.deserialize(body);
        spdlog::info("deserializedHeader {}, {}, {}, {}",
                     static_cast<int>(deserializedHeader.operation),
                     deserializedHeader.node, deserializedHeader.timestamp,
                     deserializedHeader.slot);
        spdlog::info("deserializedBody: {}, {}, {}, {}",
                     static_cast<int>(deserializedBody.state),
                     deserializedBody.timestamp, deserializedBody.slot,
                     deserializedBody.data.size());
        spdlog::info("deserializedBody[1]: {}, {}, {}",
                     deserializedBody.data[0].timestamp,
                     deserializedBody.data[0].author,
                     deserializedBody.data[0].data);

        spdlog::info("deserializedBody[2]: {}, {}, {}",
                     deserializedBody.data[1].timestamp,
                     deserializedBody.data[1].author,
                     deserializedBody.data[1].data);
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

  int counter{0};
  while (true) {
    for (auto &[address, channel] : channelStore.getRemote()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      ::serialization::BlockSerializer blockSerializer{};
      ::io::HeaderSerializer headerSerializer{};
      ::common::itf::Message message1{::common::utils::Time::getTimeStamp(),
                                      "message" + std::to_string(counter),
                                      "author" + std::to_string(counter)};
      ::common::itf::Message message2{::common::utils::Time::getTimeStamp(),
                                      "message" + std::to_string(counter + 1),
                                      "author" + std::to_string(counter + 1)};
      std::vector<::common::itf::Message> messages{};
      messages.push_back(message1);
      messages.push_back(message2);
      ::common::itf::Block block1{messages};
      const auto serializedBlock = blockSerializer.serialize(block1);
      ::io::Header header{::io::Operation::UPLOADING, configValue.self.name,
                          block1.timestamp, block1.slot};
      const auto serializedHeader = headerSerializer.serialize(header);
      const auto mergedMessage = ::io::merge(serializedHeader, serializedBlock);

      // spdlog::info("serializedHeader: {}", serializedHeader);
      // spdlog::info("serializedBlock: {}", serializedBlock);
      // spdlog::info("mergedMessage: {}", mergedMessage);
      channel->publish(mergedMessage);
    }
    counter++;
  }

  return 0;
}
