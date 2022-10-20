#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <fstream>

#include "/root/node/src/NodeConfiguration.hpp"
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <ev.h>
#include "/root/node/src/ConnectionHandler.hpp"


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

void http_server(tcp::acceptor& acceptor, tcp::socket& socket);

class http_connection : public std::enable_shared_from_this<http_connection>
{
public:

    http_connection(tcp::socket socket): socket_(std::move(socket)) {}

    void start();

private:
    tcp::socket socket_;
    beast::flat_buffer buffer_{8192};
    http::request<http::string_body> request_;
    http::response<http::dynamic_body> response_;

    void read_request();
    void process_request();
    void process_request_data(std::string name);
    void create_response();
    void write_response();
};

