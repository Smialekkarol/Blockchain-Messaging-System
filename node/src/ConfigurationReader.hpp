#pragma once

#include <optional>
#include <string>

class NodeConfiguration;

class ConfigurationReader {
public:
  static std::optional<NodeConfiguration> read(const std::string &configPath);
};