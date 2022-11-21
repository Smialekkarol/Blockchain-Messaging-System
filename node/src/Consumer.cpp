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
        handleControlMessage(message);
      });
}

Consumer::~Consumer() { ev_loop_destroy(loop); }

void Consumer::run() { ev_run(loop, 0); }

void Consumer::handleControlMessage(const std::string &data) {

  typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
  boost::char_separator<char> at("@");
  tokenizer dataTokens(data, at);

  std::vector<std::string> dataAttributes;
  BOOST_FOREACH (std::string const &token, dataTokens) {
    dataAttributes.push_back(token);
  }

  auto target = dataAttributes[0];
  auto serializedMessage = dataAttributes[1];

  serialization::MessageSerializer messageSerializer;

  common::itf::Message message =
      messageSerializer.deserialize(serializedMessage);

  std::cout << "target " << target << std::endl;
  std::cout << "message.data " << message.data << std::endl;

  if (target == "CreateChannel") {
    createChannel(message.data);
  } else if (target == "Send") {
    std::cout << message.data << std::endl;
  } else if (target == "history") {
  } else {
    spdlog::error("Invalid command");
  }
}

void Consumer::createChannel(const std::string &channelToBeCreated) {

  channel.declareQueue(channelToBeCreated, AMQP::durable)
      .onError([](const char *message) { spdlog::error(message); });

  channel.consume(channelToBeCreated, AMQP::noack)
      .onReceived([&channelToBeCreated](const AMQP::Message &msg, uint64_t tag,
                                        bool redelivered) {
        serialization::MessageSerializer messageSerializer;
        common::itf::Message messageObject =
            messageSerializer.deserialize(msg.body());
        spdlog::debug(messageObject.author + ": " + messageObject.data + "\n");
      });
}