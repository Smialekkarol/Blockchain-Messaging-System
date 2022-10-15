#pragma once

#include "NodeConfiguration.hpp"
#include <string>
#include <vector>

class Producer {
public:
  Producer(const NodeConfiguration &config);
  void run();

private:
  bool parseCommand(const std::string &command);
  void sendTo(const NodeInfo &node, const std::string &message);

  const NodeConfiguration &config;
};