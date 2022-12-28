#pragma once

#include <unordered_map>

#include "ConnectionData.hpp"

namespace io {

class ConnectionStore {
  using Address = std::string;

public:
  void push(const ::common::NodeInfo &nodeInfo);
  ConnectionData &get(const Address &address);
  std::unordered_map<Address, ConnectionData> &get();

private:
  std::unordered_map<Address, ConnectionData> connections{};
};
} // namespace io