#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <chrono>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class Client{
public:
    Client(net::io_context& ioc, std::string nodeName):resolver(ioc), httpStream(ioc), webSocketStream(ioc), nodeName(nodeName)
    {}

    int MakeInitialConnection(std::string host, char* port, std::string channelName);

    int SendMessage(std::string host, char* port, std::string queName, std::string message);

    int getData(std::string host, char* port, std::string queName, std::string date);

    int HTTPConnection(std::string host, char* port, std::string message);

    int WebSocketConnection(std::string host, char* port, std::string message);

private:
    tcp::resolver resolver;
    beast::tcp_stream httpStream;
    websocket::stream<tcp::socket> webSocketStream;
    std::string nodeName;
};