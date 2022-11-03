#pragma once

#include "common/NodeConfiguration.hpp"
#include <string>
#include <vector>

class Producer {
public:
  Producer(const common::NodeConfiguration &config);
  void run();

private:
  bool parseCommand(const std::string &command);
  void sendTo(const common::NodeInfo &node, const std::string &message);

  const common::NodeConfiguration &config;
};