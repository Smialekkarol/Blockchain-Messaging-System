#include <iostream>
#include "Client.hpp"

int main(int argc, char** argv)
{
        if(argc != 4)
        {
            std::cerr <<
                "Usage: Client <host> <port> <text>\n" <<
                "Example:\n" <<
                "    ./Client echo.websocket.org 80 \"Hello, world!\"\n";
            return EXIT_FAILURE;
        }

        std::string host = argv[1];
        const auto  port = argv[2];
        const auto  text = argv[3];
        net::io_context ioc;
        Client client = Client(ioc);
        // client.WebSocketConnection(host, port, text);
        
        client.HTTPConnection(host, port, text);
}