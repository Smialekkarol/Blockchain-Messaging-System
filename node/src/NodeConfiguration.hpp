#pragma once

#include <string>
#include <vector>

struct NodeInfo {
  std::string ip;
  std::string name;
};

struct NodeConfiguration {
  NodeInfo self;
  std::vector<NodeInfo> nodes;
};