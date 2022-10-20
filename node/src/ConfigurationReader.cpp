#include "ConfigurationReader.hpp"
#include "NodeConfiguration.hpp"
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

std::optional<NodeConfiguration>
ConfigurationReader::read(const std::string &configPath) {
  NodeConfiguration nodeConfiguration;
  try {
    auto config = YAML::LoadFile(configPath);

    nodeConfiguration.self.name = config["name"].as<std::string>();
    nodeConfiguration.self.address = config["address"].as<std::string>();

    const auto &nodes = config["known_nodes"];
    for (unsigned int i = 0; i < nodes.size(); i++) {
      const auto &name = nodes[i]["name"].as<std::string>();
      const auto &address = nodes[i]["address"].as<std::string>();

      nodeConfiguration.nodes.emplace_back(name, address);
    }
  } catch (const YAML::Exception &exception) {
    spdlog::error("Configuration error: {}", exception.what());
    return std::nullopt;
  }
  return std::make_optional(nodeConfiguration);
}