#pragma once

#include <optional>
#include <string>

namespace common {
  class NodeConfiguration;
}

class ConfigurationReader {
public:
  static std::optional<common::NodeConfiguration> read(const std::string &configPath);
};