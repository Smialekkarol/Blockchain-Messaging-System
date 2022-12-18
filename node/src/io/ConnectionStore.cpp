#include <spdlog/spdlog.h>

#include "ConnectionStore.hpp"

namespace io {
void ConnectionStore::push(const ::common::NodeInfo &nodeInfo) {
  try {
    connections.emplace(
        std::make_pair(nodeInfo.address, ConnectionData{nodeInfo}));
  } catch (const std::exception &e) {
    spdlog::error("Unable to create connection with node: {} {}, reason: {}",
                  nodeInfo.name, nodeInfo.address, e.what());
  }
}

ConnectionData &ConnectionStore::get(const std::string &address) {
  return connections.at(address);
}

std::unordered_map<std::string, ConnectionData> &ConnectionStore::get() {
  return connections;
}

} // namespace io