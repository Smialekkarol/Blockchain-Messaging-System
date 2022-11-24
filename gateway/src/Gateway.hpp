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
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <ev.h>
#include "common/serialization/HeaderSerializer.hpp"
#include "common/serialization/MessageSerializer.hpp"
#include "common/ConnectionHandler.hpp"


namespace gateway {

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http; // from <boost/beast/http.hpp>
namespace net = boost::asio; // from <boost/asio.hpp>
using tcp = net::ip::tcp; // from <boost/asio/ip/tcp.hpp>

void gatewayServer(tcp::acceptor& acceptor, tcp::socket& socket);

class GatewayConnection : public std::enable_shared_from_this<GatewayConnection> {
public:
    GatewayConnection(tcp::socket socket)
        : socket(std::move(socket))
        , loop(ev_loop_new(0))
    {
    }
    // , websocket{std::move(const_cast<tcp::socket &>(socket))}
    ~GatewayConnection(){
        std::cout<<"Gateway Destructor\n";
    }
    void start();

private:
    void read_request();
    void proccesRequestData(std::string message) const;
    void SendMessageToNode(std::string nodeAddres, common::itf::Header header, std::string message) const;
    void declareQueue(AMQP::TcpChannel& channel, std::string queName) const;
    void sendMessagesToClient(std::string message) const;

    tcp::socket socket;
    // beast::websocket::stream<tcp::socket> websocket;
    beast::flat_buffer buffer{ 8192 };
    http::request<http::string_body> request;
    http::response<http::dynamic_body> response;
    struct ev_loop* loop;
};
}