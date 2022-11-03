#include "Producer.hpp"
#include "Utils.hpp"
#include "common/ConnectionHandler.hpp"
#include "common/NodeConfiguration.hpp"
#include "common/utils/Text.hpp"
#include <algorithm>
#include <iostream>
#include <spdlog/spdlog.h>
#include <vector>

Producer::Producer(const common::NodeConfiguration &config) : config(config) {}

// this method will be removed
void Producer::run() {
  std::string command;
  bool shouldExit{false};
  while (not shouldExit) {
    std::cout << ">>> ";
    std::getline(std::cin, command);
    if (command.empty()) {
      continue;
    }
    shouldExit = parseCommand(command);
  }
}

bool Producer::parseCommand(const std::string &command) {
  auto args = common::utils::Text::tokenizeBySpace(command);
  const auto operation = common::utils::Text::toLower(args[0]);
  if (operation.compare("send") == 0) {
    if (args.size() < 3) {
      spdlog::info("send: <node_name> <message>");
      return false;
    }
    const auto &nodeName = args[1];
    const auto &message = args[2];
    const auto &node = Utils::getNodeByName(nodeName, config);
    if (not node.has_value()) {
      spdlog::info("send: node not registered");
      return false;
    }
    sendTo(node.value(), message);
  } else if (operation.compare("broadcast") == 0) {
    if (args.size() < 2) {
      spdlog::info("broadcast: <message>");
      return false;
    }
    const auto &message = args[1];
    for (const auto &node : config.nodes) {
      sendTo(node, message);
    }
  } else if (operation.compare("exit") == 0) {
    return true;
  } else if (operation.compare("help") == 0) {
    spdlog::info("Possible commands: [send, broadcast, exit]");
  } else {
    spdlog::info("Unknown command");
  }
  return false;
}

void Producer::sendTo(const common::NodeInfo &node,
                      const std::string &message) {
  spdlog::debug("Sending message to (name:{}, address:{})", node.name,
                node.address);
  auto *loop = ev_loop_new(0);
  common::ConnectionHandler handler(loop);

  AMQP::TcpConnection connection(&handler, AMQP::Address(node.address));
  AMQP::TcpChannel channel(&connection);
  channel.onError(
      [](const char *message) { spdlog::error("Channel error: {}", message); });
  channel.declareQueue(node.name, AMQP::durable)
      .onSuccess([&connection, &channel, &node,
                  &message](const std::string &name, uint32_t messagecount,
                            uint32_t consumercount) {
        channel.publish("", node.name, message);
      })
      .onError([](const char *message) { spdlog::error(message); })
      .onFinalize([&connection]() { connection.close(); });
  ev_run(loop, 0);
}