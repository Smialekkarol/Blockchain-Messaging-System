#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "Beast.hpp"
#include "common/serialization/HeaderSerializer.hpp"
#include "common/serialization/MessageSerializer.hpp"
#include "common/Buffer.hpp"

//refactor klasy

namespace gateway {

void fail(beast::error_code ec, char const *what);

class Session : public std::enable_shared_from_this<Session> {
public:
  explicit Session(tcp::socket &&socket);

  void run();

  void onRun();

  void onAccept(beast::error_code ec);

  void doRead();

  void onRead(beast::error_code ec, std::size_t bytes_transferred);

  void onWrite(beast::error_code ec, std::size_t bytes_transferred);

  void proccesRequestData(std::string& data);

  void SendMessageToNode(std::string nodeAddres, common::itf::Header header, std::string message);

private:
  websocket::stream<beast::tcp_stream> websocket;
  beast::flat_buffer buffer;
  struct ev_loop* loop;
  common::Buffer<std::string> messageBuffer{};
};
}