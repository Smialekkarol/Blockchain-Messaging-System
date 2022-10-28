#include <iostream>
#include "Client.hpp"
#include "Utils.hpp"

int main(int argc, char** argv)
{
        if(argc != 4)
        {
            std::cerr <<
                "Usage: Client <host> <port> <ServerName>\n" <<
                "Example:\n" <<
                "    ./Client 127.0.0.1 8080 NodeA";
            return EXIT_FAILURE;
        }

        std::string host = argv[1];
        const auto  port = argv[2];
        const auto  serverName = argv[3];
        net::io_context ioc;
        Client client = Client(ioc, serverName);

        std::string command;
        bool shouldExit{false};
        while (not shouldExit) {
            std::cout << ">>> ";
            std::getline(std::cin, command);
            if (command.empty()) {
            continue;
            }
            auto args = tokenizeBySpace(command);
            const auto operation = toLower(args[0]);

            if (operation.compare("init") == 0) 
            {
                if (args.size() == 2) {
                const auto &queToBeCreated = args[1];
                client.MakeInitialConnection(host, port, queToBeCreated);
                }
                else
                 std::cout<<"initChannel: <queToBeCreated>\n";
            }

            if (operation.compare("send") == 0) 
            {
                if (args.size() == 3) {
                const auto &queName = args[1];
                const auto &message = args[2];
                client.SendMessage(host, port, queName, message);

                }
                else
                  std::cout<<"send: <queName> <message>\n";
            } 
            
            if(operation.compare("getdata") == 0)
            {
                if(args.size() == 3)
                {
                   const auto &queName = args[1];
                   const auto &date = args[2];
                   client.getData(host, port, queName, date);
                }
                else 
                  std::cout<<"getData: <queName> <date>\n";
            }

            else if (operation.compare("exit") == 0) {
                shouldExit = true;
            } 
            else if (operation.compare("help") == 0) {
                std::cout<<"Possible commands: [init, send, exit]\n";
            }
            }
}