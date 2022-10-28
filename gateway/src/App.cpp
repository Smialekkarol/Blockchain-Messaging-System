#include "HTTPServer.hpp"
#include "WebSocketServer.hpp"

// int main(int argc, char* argv[])
// {
//     try
//     {
//         if(argc != 3)
//         {
//             std::cerr << "Usage: " << argv[0] << " <address> <port>\n";
//             std::cerr << "  For IPv4, try:\n";
//             std::cerr << "    receiver 0.0.0.0 80\n";
//             std::cerr << "  For IPv6, try:\n";
//             std::cerr << "    receiver 0::0 80\n";
//             return EXIT_FAILURE;
//         }

//         auto const address = net::ip::make_address(argv[1]);
//         unsigned short port = static_cast<unsigned short>(std::atoi(argv[2]));

//         net::io_context ioc{1};

//         tcp::acceptor acceptor{ioc, {address, port}};
//         tcp::socket socket{ioc};
//         http_server(acceptor, socket);

//         ioc.run();
//     }
//     catch(std::exception const& e)
//     {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return EXIT_FAILURE;
//     }
// }


int main()
{
    auto const address = boost::asio::ip::make_address("127.0.0.1");
    auto const port = static_cast<short unsigned int>(std::stoi("8080"));


        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {address, port}};
        tcp::socket socket{ioc};
        websocket_server(acceptor, socket);

        ioc.run();
   
}