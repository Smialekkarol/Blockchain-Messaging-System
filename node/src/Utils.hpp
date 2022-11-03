#pragma once

#include <optional>
#include <string>

namespace common {
class NodeInfo;
class NodeConfiguration;
}

class Utils {
public:
  static std::optional<common::NodeInfo>
  getNodeByAddress(const std::string &address, const common::NodeConfiguration &config);
  static std::optional<common::NodeInfo> getNodeByName(const std::string &name,
                                               const common::NodeConfiguration &config);
};