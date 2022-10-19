#pragma once

#include <optional>
#include <string>
#include <vector>

class NodeInfo;
class NodeConfiguration;

class Utils {
public:
  static std::optional<NodeInfo>
  getNodeByAddress(const std::string &address, const NodeConfiguration &config);
  static std::optional<NodeInfo> getNodeByName(const std::string &name,
                                               const NodeConfiguration &config);
  static std::string toLower(const std::string &str);
  static std::vector<std::string> tokenizeBySpace(const std::string &command);
};