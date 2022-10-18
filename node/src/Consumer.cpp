#include "Consumer.hpp"
#include "ConnectionHandler.hpp"
#include <amqpcpp.h>
#include <thread>

void Consumer::run(const NodeConfiguration &config) {
  std::thread{Consumer::main, config}.detach();
}

void Consumer::main(const NodeConfiguration &config) {
  auto *loop = ev_loop_new(0);
  ConnectionHandler handler(loop);
  AMQP::TcpConnection connection(&handler, AMQP::Address(config.self.ip));
  AMQP::TcpChannel channel(&connection);
  channel.onError([](const char *message) {
    std::cout << "Channel error: " << message << std::endl;
  });
  channel.declareQueue(config.self.name, AMQP::durable)
      .onError([](const char *message) { std::cout << message << "\n"; });
  channel.consume(config.self.name, AMQP::noack)
      .onReceived([](const AMQP::Message &msg, uint64_t tag, bool redelivered) {
        std::cout << "Received: " << msg.message() << std::endl;
      });
  ev_run(loop, 0);
}