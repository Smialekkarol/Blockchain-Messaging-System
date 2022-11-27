#include "Buffer.hpp"
#include "ConfigurationReader.hpp"
#include "Consumer.hpp"
#include "common/NodeConfiguration.hpp"
#include "common/utils/Time.hpp"
#include "db/RedisDB.hpp"
#include "slot/SlotHandler.hpp"
#include "slot/TimerWheel.hpp"
#include <spdlog/spdlog.h>

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
  timerWheel.start();

  std::thread{ [&consumer]() { consumer.run(); } }.detach();

  while (true) {
  }

  return 0;
}
