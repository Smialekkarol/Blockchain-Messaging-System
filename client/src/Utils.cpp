#include "Utils.hpp"
#include <algorithm>
#include <iostream>
#include <spdlog/spdlog.h>
#include <sstream>
#include <yaml-cpp/yaml.h>

std::vector<std::string> tokenizeBySpace(const std::string &command) {
  std::vector<std::string> args;
  std::string tmp;
  auto iss = std::istringstream(command);
  while (iss >> tmp) {
    args.push_back(tmp);
  }
  return args;
}

std::string toLower(const std::string &str) {
  std::string lower{str};
  std::transform(lower.begin(), lower.end(), lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return lower;
}

std::string readNodes(const std::string &configPath,
                      const std::string &nodeName) {
  try {
    std::cout << "configPath: " << configPath << " nodeName: " << nodeName
              << std::endl;
    auto config = YAML::LoadFile(configPath);
    std::string nodeAddress{};
    const auto &nodes = config["nodes"];
    for (unsigned int i = 0; i < nodes.size(); i++) {
      if (nodes[i]["name"].as<std::string>() == nodeName) {
        return nodes[i]["address"].as<std::string>();
      }
    }
  } catch (const YAML::Exception &exception) {
    spdlog::error("Configuration error: {}", exception.what());
    return "";
  }
  return "";
}