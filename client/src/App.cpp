#include "Client.hpp"
#include "Utils.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "common/itf/Constants.hpp"
#include "common/utils/Time.hpp"
#include "common/utils/Timer.hpp"

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "Usage: Client <ServerName> <ClientName>\n"
              << "Example:\n"
              << "    ./Client NodeA ClientName";
    return EXIT_FAILURE;
  }

  const auto &serverName = argv[1];
  const auto &clientName = argv[2];

  spdlog::set_level(spdlog::level::debug);
  auto defaultFileLogger =
      spdlog::basic_logger_mt("default_logger", "logs/client/logs.txt", true);
  spdlog::set_default_logger(defaultFileLogger);

  const std::string &nodeAddress = readNodes("Nodes.yaml", serverName);

  client::ClientInfo clientInfo(nodeAddress, serverName, clientName);
  client::Client client(clientInfo);

  const std::uint32_t numberOfClients{3};
  std::vector<std::shared_ptr<client::Client>> clients{};

  for (std::uint32_t i{0}; i < numberOfClients; i++) {
    client::ClientInfo clientInfoLocal(nodeAddress, serverName,
                                       std::string{clientName} + "_" +
                                           std::to_string(i));
    clients.push_back(std::make_shared<client::Client>(clientInfoLocal));
  }

  std::string command{};

  std::uint64_t counter{0};
  client.createQueue(::common::itf::DEFAULT_BLOCKAIN);
  ::common::utils::Timer timer{};
  timer.setInterval(
      [&clients, &counter]() {
        for (auto &c : clients) {
          c->sendMessage(::common::itf::DEFAULT_BLOCKAIN,
                         "message" + std::to_string(counter));
        }
        counter++;
      },
      ::common::utils::Time::SLOT);

  bool shouldExit{false};
  while (not shouldExit) {
    std::cout << ">>> ";
    std::getline(std::cin, command);
    if (command.empty()) {
      continue;
    }
    auto args = tokenizeBySpace(command);
    const auto operation = toLower(args[0]);

    if (operation.compare("init") == 0) {
      if (args.size() == 2) {
        const auto &queToBeCreated = args[1];
        client.createQueue(queToBeCreated);
      } else
        std::cout << "initChannel: <queToBeCreated>\n";
    }

    if (operation.compare("send") == 0) {
      if (args.size() == 3) {
        const std::string &queName = args[1];
        const std::string &message = args[2];
        client.sendMessage(queName, message);
      } else
        std::cout << "send: <queName> <message>\n";
    }

    if (operation.compare("getMessages") == 0) {
      if (args.size() == 3) {
        const auto &queName = args[1];
        const auto &date = args[2];
        client.getMessages(queName, date);
      } else
        std::cout << "getMessages: <queName> <date>\n";
    }

    else if (operation.compare("exit") == 0) {
      shouldExit = true;
    } else if (operation.compare("help") == 0) {
      std::cout << "Possible commands: [init, send, exit]\n";
    }
  }

  return 0;
}
