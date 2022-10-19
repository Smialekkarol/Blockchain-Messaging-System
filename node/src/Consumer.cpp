#include "Consumer.hpp"
#include "ConnectionHandler.hpp"
#include <amqpcpp.h>
#include <spdlog/spdlog.h>
#include <thread>

void Consumer::run(const NodeConfiguration &config) {
  std::thread{Consumer::main, config}.detach();
}

void Consumer::main(const NodeConfiguration &config) {
  auto *loop = ev_loop_new(0);
  ConnectionHandler handler(loop);
  AMQP::TcpConnection connection(&handler, AMQP::Address(config.self.address));
  AMQP::TcpChannel channel(&connection);
  channel.onError(
      [](const char *message) { spdlog::error("Channel error: {}", message); });
  channel.declareQueue(config.self.name, AMQP::durable)
      .onError([](const char *message) { spdlog::error(message); });
  channel.consume(config.self.name, AMQP::noack)
      .onReceived([](const AMQP::Message &msg, uint64_t tag, bool redelivered) {
        std::string message{msg.body(), msg.body() + msg.bodySize()};
        spdlog::debug("Received: {}", message);
      });
  ev_run(loop, 0);
}