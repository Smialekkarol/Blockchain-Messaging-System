#pragma once

#include "common/ConnectionHandler.hpp"
#include "common/NodeConfiguration.hpp"
#include "common/serialization/MessageSerializer.hpp"

#include <amqpcpp.h>
#include <ev.h>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

class Consumer {
public:
  Consumer(const common::NodeConfiguration &config);
  Consumer(const Consumer &) = delete;
  Consumer(Consumer &&) = delete;
  Consumer &operator=(const Consumer &) = delete;
  Consumer &operator=(Consumer &&) = delete;
  ~Consumer();

  static void run(const common::NodeConfiguration &config);

private:
  struct ev_loop *loop;
  common::ConnectionHandler handler;
  AMQP::TcpConnection connection;
  AMQP::TcpChannel channel;

  void run();
  void handleControlMessage(const std::string &command);
  void createChannel(const std::string &command);
};
