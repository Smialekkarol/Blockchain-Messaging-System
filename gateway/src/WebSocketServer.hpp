#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>


class websocket_connection: public std::enable_shared_from_this<websocket_connection>
{
public:
    websocket_connection(tcp::socket socket): socket(std::move(socket)) {}

    void start();
    

private:
    tcp::socket socket;
    beast::flat_buffer buffer{8192};
    http::request<http::string_body> request;
    http::response<http::dynamic_body> response;

    void read_request();
};