#include <iostream>

#include "ConfigurationReader.hpp"
#include "Consumer.hpp"
#include "NodeConfiguration.hpp"
#include "Producer.hpp"

void printConfiguration(const NodeConfiguration &config) {
  const auto &nodes{config.nodes};
  std::cout << "Loaded " << nodes.size() << " node(s):\n";
  for (const auto &node : nodes) {
    std::cout << "ip: " << node.ip << ", name: " << node.name << "\n";
  }
}

/*
  node name = node queue
*/
int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << argv[0] << ": <node_configuration_file>\n";
    return 1;
  }
  const auto &config = ConfigurationReader::read(argv[1]);
  if (not config.has_value()) {
    std::cerr << "Failed to read configuration\n";
    return 2;
  }
  const auto &configValue = config.value();
  printConfiguration(configValue);

  Consumer::run(configValue);
  Producer{configValue}.run();
  return 0;
}
