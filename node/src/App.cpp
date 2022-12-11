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
        spdlog::info("[{}]{} Received: {}, {}", localNodeInfo.address,
                     localNodeInfo.name, msg.body(), tag);
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
      const std::string message{configValue.self.name + " sending message nr." +
                                std::to_string(counter) + " to " +
                                channel->node};
      channel->publish(message);
    }
    counter++;
  }

  return 0;
}
