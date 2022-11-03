#include "WebSocketServer.hpp"

int main() {
  auto const address = boost::asio::ip::make_address("127.0.0.1");
  auto const port = static_cast<short unsigned int>(std::stoi("8080"));

  net::io_context ioc{1};
  tcp::acceptor acceptor{ioc, {address, port}};
  tcp::socket socket{ioc};
  websocket_server(acceptor, socket);

  ioc.run();
}