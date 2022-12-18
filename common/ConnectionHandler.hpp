#pragma once

#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <ev.h>
#include <spdlog/spdlog.h>

namespace common {
class ConnectionHandler : public AMQP::LibEvHandler {
public:
  using super = AMQP::LibEvHandler;
  using super::super;

private:
  virtual void onError(AMQP::TcpConnection *connection,
                       const char *message) override {
    spdlog::error(message);
  }

  virtual void onConnected(AMQP::TcpConnection *connection) override {}

  virtual void onClosed(AMQP::TcpConnection *connection) override {}
};
} // namespace common