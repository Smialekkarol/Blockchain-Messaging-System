#include "ConfigurationReader.hpp"
#include "NodeConfiguration.hpp"
#include <yaml-cpp/yaml.h>

std::optional<NodeConfiguration>
ConfigurationReader::read(const std::string &configPath) {
  // TODO: add proper error handling
  NodeConfiguration nodeConfiguration;
  auto config = YAML::LoadFile(configPath);

  if ((not config["name"].IsDefined()) or (not config["address"].IsDefined())) {
    return std::nullopt;
  }

  nodeConfiguration.self.name = config["name"].as<std::string>();
  nodeConfiguration.self.address = config["address"].as<std::string>();

  const auto &nodes = config["known_nodes"];
  for (unsigned int i = 0; i < nodes.size(); i++) {
    const auto &name = nodes[i]["name"].as<std::string>();
    const auto &address = nodes[i]["address"].as<std::string>();

    nodeConfiguration.nodes.emplace_back(name, address);
  }
  return std::make_optional(nodeConfiguration);
}