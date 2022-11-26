#include "WebsocketHandler.hpp"

namespace client {

WebsocketHandler::WebsocketHandler(net::io_context &ioc)
    : resolver(net::make_strand(ioc)), websocket(net::make_strand(ioc)) {}

WebsocketHandler::~WebsocketHandler() {
  websocket.close(websocket::close_code::normal);
}

int WebsocketHandler::EstablishConnection(ClientInfo &clientInfo) {
  try {
    auto const results = resolver.resolve(clientInfo.host, clientInfo.port);
    auto ep = net::connect(websocket.next_layer(), results);
    auto address = clientInfo.host + ':' + std::to_string(ep.port());
    websocket.handshake(address, "/");

  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int WebsocketHandler::writeDataToWebsocket(std::string message) {
  try {
    websocket.write(net::buffer(message));
  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int WebsocketHandler::readDataFromWebsocket() {
     try {
    websocket.read(buffer);
     } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void WebsocketHandler::clearBuffer() { buffer.consume(buffer.size()); }

std::string WebsocketHandler::getBufferedData() {
  return boost::beast::buffers_to_string(buffer.data());
}
int WebsocketHandler::getBufferedDataSize() { return buffer.size(); }
}