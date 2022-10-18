#include <iostream>

#include "Consumer.hpp"
#include "NodeConfiguration.hpp"
#include "Producer.hpp"

std::vector<NodeInfo> parseNodeInfo(const std::vector<std::string> &args) {
  std::vector<NodeInfo> nodes;
  for (const auto &s : args) {
    const auto separatorIndex = s.find_first_of(" ");
    if (separatorIndex == std::string::npos or
        s.substr(separatorIndex).size() == 1) {
      std::cout << "Argument \"" << s << "\" is invalid, skipping\n";
      continue;
    }
    const auto ip{s.substr(0, separatorIndex)};
    const auto name{s.substr(separatorIndex + 1)};
    nodes.push_back(NodeInfo{ip, name});
  }
  return nodes;
}

void printConfiguration(const NodeConfiguration &config) {
  const auto &nodes{config.nodes};
  std::cout << "Loaded " << nodes.size() << " node(s):\n";
  for (const auto &node : nodes) {
    std::cout << "ip: " << node.ip << ", name: " << node.name << "\n";
  }
}

/*
  node name = node queue
*/
int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout
        << argv[0]
        << ": <current_node_name> \"[<ip1> <remote_node_name1>]\" \"[<ip2> "
           "<remote_node_name2>]\" ...\n";
    return 1;
  }
  NodeInfo currentNode{"amqp://localhost/", argv[1]};
  std::vector<std::string> args{&argv[2], &argv[argc]};
  NodeConfiguration config{currentNode, parseNodeInfo(args)};
  printConfiguration(config);

  Consumer::run(config);
  Producer{config}.run();
  return 0;
}
