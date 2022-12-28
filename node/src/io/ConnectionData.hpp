#pragma once

#include <memory>
#include <string>

#include <amqpcpp.h>

#include "common/ConnectionHandler.hpp"
#include "common/NodeConfiguration.hpp"

namespace io {
struct ConnectionData {
  ConnectionData(const ::common::NodeInfo &nodeInfo);
  ConnectionData(ConnectionData &&other);
  ConnectionData &operator=(ConnectionData &&other);
  ConnectionData(const ConnectionData &other) = delete;
  ConnectionData &operator=(const ConnectionData &other) = delete;
  ~ConnectionData();

  struct ev_loop *loop;
  std::unique_ptr<common::ConnectionHandler> handler{};
  std::unique_ptr<AMQP::TcpConnection> connection{};
  std::string node{};
};
} // namespace io