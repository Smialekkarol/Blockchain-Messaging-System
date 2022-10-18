#include "Producer.hpp"
#include "ConnectionHandler.hpp"
#include "NodeConfiguration.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

Producer::Producer(const NodeConfiguration &config) : config(config) {}

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
  auto args = Utils::tokenizeBySpace(command);
  const auto operation = Utils::toLower(args[0]);
  if (operation.compare("send") == 0) {
    if (args.size() < 3) {
      std::cout << "send: <node_name> <message>\n";
      return false;
    }
    const auto &nodeName = args[1];
    const auto &message = args[2];
    const auto &node = Utils::getNodeByName(nodeName, config);
    if (not node.has_value()) {
      std::cout << "send: node not registered\n";
      return false;
    }
    sendTo(node.value(), message);
  } else if (operation.compare("broadcast") == 0) {
    if (args.size() < 2) {
      std::cout << "broadcast: <message>\n";
      return false;
    }
    const auto &message = args[1];
    for (const auto &node : config.nodes) {
      sendTo(node, message);
    }
  } else if (operation.compare("exit") == 0) {
    return true;
  } else if (operation.compare("help") == 0) {
    std::cout << "Possible commands: [send, broadcast, exit]\n";
  } else {
    std::cout << "Unknown command\n";
  }
  return false;
}

void Producer::sendTo(const NodeInfo &node, const std::string &message) {
  std::cout << "Sending message to (name:" << node.name << ", ip:" << node.ip
            << ")\n";
  auto *loop = ev_loop_new(0);
  ConnectionHandler handler(loop);

  AMQP::TcpConnection connection(&handler, AMQP::Address(node.ip));
  AMQP::TcpChannel channel(&connection);
  channel.onError([](const char *message) {
    std::cout << "Channel error: " << message << std::endl;
  });
  channel.declareQueue(node.name, AMQP::durable)
      .onSuccess([&connection, &channel, &node,
                  &message](const std::string &name, uint32_t messagecount,
                            uint32_t consumercount) {
        channel.publish("", node.name, message);
      })
      .onError([](const char *message) { std::cout << message << "\n"; })
      .onFinalize([&connection]() { connection.close(); });
  ev_run(loop, 0);
}