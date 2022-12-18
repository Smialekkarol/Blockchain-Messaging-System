#include "Utils.hpp"
#include <iostream>
#include "Client.hpp"


int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: Client <ServerName> <ClientName>\n"
                  << "Example:\n"
                  << "    ./Client NodeA ClientName";
        return EXIT_FAILURE;
    }

    const auto& serverName = argv[1];
    const auto& clientName = argv[2];

    const std::string & nodeAddress = readNodes("Nodes.yaml", serverName);

    client::ClientInfo clientInfo(nodeAddress, serverName, clientName);
    client::Client client(clientInfo);

    std::string command{};

    bool shouldExit{ false };
    while (not shouldExit)
    {
        std::cout << ">>> ";
        std::getline(std::cin, command);
        if (command.empty())
        {
            continue;
        }
        auto args = tokenizeBySpace(command);
        const auto operation = toLower(args[0]);

        if (operation.compare("init") == 0)
        {
            if (args.size() == 2)
            {
                const auto& queToBeCreated = args[1];
                client.createQueue(queToBeCreated);
            }
            else
                std::cout << "initChannel: <queToBeCreated>\n";
        }

        if (operation.compare("send") == 0)
        {
            if (args.size() == 3)
            {
                const std::string & queName = args[1];
                const std::string & message = args[2];
                client.sendMessage(queName, message);
            }
            else
                std::cout << "send: <queName> <message>\n";
        }

        if (operation.compare("getMessages") == 0)
        {
            if (args.size() == 3)
            {
                const auto& queName = args[1];
                const auto& date = args[2];
                client.getMessages(queName, date);
            }
            else
                std::cout << "getMessages: <queName> <date>\n";
        }

        else if (operation.compare("exit") == 0)
        {
            shouldExit = true;
        }
        else if (operation.compare("help") == 0)
        {
            std::cout << "Possible commands: [init, send, exit]\n";
        }
    }

    return 0;
}
