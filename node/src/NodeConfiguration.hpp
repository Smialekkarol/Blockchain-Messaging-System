#pragma once

#include <string>
#include <vector>

struct NodeInfo {
  NodeInfo() = default;
  NodeInfo(const std::string &ip, const std::string &name)
      : ip(ip), name(name) {}

  std::string ip;
  std::string name;
};

struct NodeConfiguration {
  NodeInfo self;
  std::vector<NodeInfo> nodes;
};