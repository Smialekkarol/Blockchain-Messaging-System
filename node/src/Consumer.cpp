#include "Consumer.hpp"
#include "Utils.hpp"
#include "common/utils/Text.hpp"
#include <amqpcpp.h>
#include <spdlog/spdlog.h>
#include <thread>

void Consumer::run(const common::NodeConfiguration &config) {
  std::thread{[&config]() { Consumer{config}.run(); }}.detach();
}

Consumer::Consumer(const common::NodeConfiguration &config)
    : loop(ev_loop_new(0)), handler(loop),
      connection(&handler, AMQP::Address(config.self.address)),
      channel(&connection) {
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
        auto tokens = common::utils::Text::tokenizeBySpace(message);
        handleControlMessage(tokens);
      });
}

Consumer::~Consumer() { ev_loop_destroy(loop); }

void Consumer::run() { ev_run(loop, 0); }

void Consumer::handleControlMessage(const gsl::span<std::string> &command) {
  const auto &numberOfArgs = command.size();
  if (numberOfArgs == 0) {
    spdlog::error("Empty message");
    return;
  }
  const auto &operation = command[0];
  const auto &args = command.subspan(1, command.size() - 1);
  if (operation == "create") {
    createChannel(args);
  } else if (operation == "history") {
    //
  } else {
    spdlog::error("Invalid command");
  }
}

void Consumer::createChannel(const gsl::span<std::string> &command) {
  if (command.size() == 0) {
    spdlog::error("Invalid number of args");
    return;
  }
  std::string channelToBeCreated = command[0];
  channel.declareQueue(channelToBeCreated, AMQP::durable)
      .onError([](const char *message) { spdlog::error(message); });
  channel.consume(channelToBeCreated, AMQP::noack)
      .onReceived([&channelToBeCreated](const AMQP::Message &msg, uint64_t tag,
                                        bool redelivered) {
        std::string message{msg.body(), msg.body() + msg.bodySize()};
        spdlog::debug("Received on channel \"{}\": {}", channelToBeCreated,
                      message);
      });
}