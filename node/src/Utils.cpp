#include "Utils.hpp"
#include "common/NodeConfiguration.hpp"
#include <algorithm>

std::optional<common::NodeInfo>
Utils::getNodeByAddress(const std::string &address,
                        const common::NodeConfiguration &config) {
  auto it = std::find_if(config.nodes.begin(), config.nodes.end(),
                         [address](const common::NodeInfo &node) {
                           return address == node.address;
                         });
  if (it != config.nodes.end()) {
    return std::make_optional(*it);
  }
  return std::nullopt;
}

std::optional<common::NodeInfo>
Utils::getNodeByName(const std::string &name,
                     const common::NodeConfiguration &config) {
  auto it = std::find_if(
      config.nodes.begin(), config.nodes.end(),
      [name](const common::NodeInfo &node) { return name == node.name; });
  if (it != config.nodes.end()) {
    return std::make_optional(*it);
  }
  return std::nullopt;
}