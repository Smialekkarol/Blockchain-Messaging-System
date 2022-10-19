#pragma once

#include <string>
#include <vector>

struct NodeInfo {
  NodeInfo() = default;
  NodeInfo(const std::string &name, const std::string &address)
      : name(name), address(address) {}

  std::string address;
  std::string name;
};

struct NodeConfiguration {
  NodeInfo self;
  std::vector<NodeInfo> nodes;
};