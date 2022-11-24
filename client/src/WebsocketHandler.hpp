#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <chrono>

#include "ClientInfo.hpp"

namespace client {

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class WebsocketHandler {
public:
  WebsocketHandler(net::io_context &ioc);

  ~WebsocketHandler();

  int EstablishConnection(ClientInfo &clientInfo);

  int writeDataToWebsocket(std::string message);

  int readDataFromWebsocket();

  void clearBuffer();

  std::string getBufferedData();

  int getBufferedDataSize();
private:
  websocket::stream<tcp::socket> websocket;
  beast::flat_buffer buffer;
  tcp::resolver resolver;
};
}