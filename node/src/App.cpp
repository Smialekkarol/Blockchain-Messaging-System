#include "Buffer.hpp"
#include "ConfigurationReader.hpp"
#include "Consumer.hpp"
#include "Producer.hpp"
#include "common/NodeConfiguration.hpp"
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
  // TODO add awareness of slot and handling of empty buffer
  // The Block and message needs slot field.
  // TimerWheel needs to start at the begining of the nearest slot
  // Pending and Complete callback need to now it's slots
  ::db::RedisDB redis{};
  Buffer buffer{};
  ::slot::TimerWheel timerWheel{};
  ::slot::SlotHandler slotHandler{redis, buffer};
  timerWheel.subscribe([&slotHandler]() { slotHandler.handle(); });

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

  Consumer::run(configValue);
  Producer{configValue}.run();
  return 0;
}
