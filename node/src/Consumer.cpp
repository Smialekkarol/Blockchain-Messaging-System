#include "Consumer.hpp"
#include "Utils.hpp"
#include "common/ConnectionHandler.hpp"
#include <amqpcpp.h>
#include <spdlog/spdlog.h>
#include <thread>

void Consumer::run(const common::NodeConfiguration &config) {
  std::thread{Consumer::main, config}.detach();
}

void Consumer::main(const common::NodeConfiguration &config) {
  auto *loop = ev_loop_new(0);
  common::ConnectionHandler handler(loop);
  AMQP::TcpConnection connection(&handler, AMQP::Address(config.self.address));
  std::string channelToBeCreated;
  AMQP::TcpChannel channel(&connection);
  channel.onError(
      [](const char *message) { spdlog::error("Channel error: {}", message); });

  channel.declareQueue(config.self.name + "_ControlChannel", AMQP::durable)
      .onError([](const char *message) { spdlog::error(message); });

  channel.consume(config.self.name + "_ControlChannel", AMQP::noack)
      .onReceived([&](const AMQP::Message &msg, uint64_t tag,
                      bool redelivered) {
        std::string message{msg.body(), msg.body() + msg.bodySize()};
        spdlog::debug(config.self.name + "_ControlChannel" + " Received: {}",
                      message);
        channelToBeCreated = message;
        channel.declareQueue(channelToBeCreated, AMQP::durable)
            .onError([](const char *message) { spdlog::error(message); });
      });

  ev_run(loop, 0);
}