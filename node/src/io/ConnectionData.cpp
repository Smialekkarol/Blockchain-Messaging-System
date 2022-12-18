#include <utility>

#include <ev.h>

#include "ConnectionData.hpp"

namespace io {
ConnectionData::ConnectionData(const ::common::NodeInfo &nodeInfo)
    : loop{ev_loop_new(0)}, handler{std::make_unique<common::ConnectionHandler>(
                                loop)},
      connection{std::make_unique<AMQP::TcpConnection>(
          handler.get(), AMQP::Address(nodeInfo.address))},
      node{nodeInfo.name} {};

ConnectionData::ConnectionData(ConnectionData &&other) {
  loop = other.loop;
  other.loop = nullptr;
  handler = std::move(other.handler);
  connection = std::move(other.connection);
  node = std::move(other.node);
};

ConnectionData &ConnectionData::operator=(ConnectionData &&other) {
  loop = other.loop;
  other.loop = nullptr;
  handler = std::move(other.handler);
  connection = std::move(other.connection);
  node = std::move(other.node);
  return *this;
};

ConnectionData::~ConnectionData() { ev_loop_destroy(loop); }
} // namespace io