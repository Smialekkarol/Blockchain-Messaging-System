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
#include "GatewayAsync.hpp"

namespace gateway {

class Listener : public std::enable_shared_from_this<Listener> {
public:
  Listener(net::io_context &ioc, tcp::endpoint endpoint);

  void run();

private:
  void do_accept();

  void on_accept(beast::error_code ec, tcp::socket socket);

  net::io_context &ioc_;
  tcp::acceptor acceptor_;
};
}