#include "Utils.hpp"
#include "NodeConfiguration.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

std::optional<NodeInfo>
Utils::getNodeByAddress(const std::string &address,
                        const NodeConfiguration &config) {
  auto it = std::find_if(
      config.nodes.begin(), config.nodes.end(),
      [address](const NodeInfo &node) { return address == node.address; });
  if (it != config.nodes.end()) {
    return std::make_optional(*it);
  }
  return std::nullopt;
}

std::optional<NodeInfo> Utils::getNodeByName(const std::string &name,
                                             const NodeConfiguration &config) {
  auto it =
      std::find_if(config.nodes.begin(), config.nodes.end(),
                   [name](const NodeInfo &node) { return name == node.name; });
  if (it != config.nodes.end()) {
    return std::make_optional(*it);
  }
  return std::nullopt;
}

std::string Utils::toLower(const std::string &str) {
  std::string lower{str};
  std::transform(lower.begin(), lower.end(), lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return lower;
}

std::vector<std::string> Utils::tokenizeBySpace(const std::string &command) {
  std::vector<std::string> args;
  std::string tmp;
  auto iss = std::istringstream(command);
  while (iss >> tmp) {
    args.push_back(tmp);
  }
  return args;
}