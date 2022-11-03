#include "WebSocketServer.hpp"
#include <string.h>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

#include "common/NodeConfiguration.hpp"
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <ev.h>
#include "common/ConnectionHandler.hpp"

typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

void websocket_connection::start()
{
    read_request();
}

void websocket_connection::read_request()
{
    std::thread{
            [q{ std::move(socket) }, this](){

                beast::websocket::stream<tcp::socket> websocket{ std::move(const_cast<tcp::socket&>(q)) };
    websocket.accept();
    while (1) {
        try {
            beast::flat_buffer buffer;
            websocket.read(buffer);
            auto out = beast::buffers_to_string(buffer.cdata());
            this->proccesRequestData(out);
            websocket.write(buffer.data());
        }
        catch (beast::system_error const& error) {
            if (error.code() == beast::websocket::error::closed) {
                break;
            }
        }
    }
}
}
.detach();
}

void websocket_connection::proccesRequestData(std::string data) const
{
    boost::char_separator<char> sep(";");
    boost::char_separator<char> at("@");

    tokenizer dataTokens(data, at);
    std::vector<std::string> dataAttributes;
    BOOST_FOREACH (std::string const& token, dataTokens) {
        dataAttributes.push_back(token);

    }

    auto header = dataAttributes[0];
    auto message = dataAttributes[1];

    tokenizer headerTokens(header, sep);
    std::vector<std::string> headerAttributes;
    BOOST_FOREACH (std::string const& token, headerTokens) {
        headerAttributes.push_back(token);
    }

    auto target = headerAttributes[0];
    auto nodeName = headerAttributes[1];
    auto queName = headerAttributes[2];

    if (target == "CreateChannel") {
        this->SendMessage("amqp://localhost/", nodeName, queName, message);
        std::cout<<"CreateChannel request received\n";
    }
    if(target == "Send"){
        this->SendMessage("amqp://localhost/", nodeName, queName, message);
        std::cout<<"Send request received\n"<<nodeName;
    }
    if(target == "GetData"){
        std::cout<<"GetData request received\n";
    }
}

void websocket_connection::SendMessage(std::string nodeAddress, std::string nodeName, std::string queName, std::string message) const{

    common::NodeInfo node{ nodeAddress, nodeName };
    auto* loop = ev_loop_new(0);
    common::ConnectionHandler handler(loop);

    AMQP::TcpConnection connection(&handler, AMQP::Address(nodeAddress));
    AMQP::TcpChannel channel(&connection);
    channel.onError([](const char* message) {
        std::cout << "Channel error: " << message << std::endl;
    });
    std::cout<<"message sent on "<<queName<<std::endl;
    channel.declareQueue(queName, AMQP::durable)
        .onSuccess([&connection, &channel, &node,
            &message](const std::string& name, uint32_t messagecount,
            uint32_t consumercount) {
            channel.publish("", name, message);
            
        })
        .onError([](const char* message) { std::cout << message << "\n"; })
        .onFinalize([&connection]() { connection.close(); });
    ev_run(loop, 0);
}

void websocket_server(tcp::acceptor& acceptor, tcp::socket& socket)
{
    acceptor.async_accept(socket,
        [&](beast::error_code ec) {
            if (!ec)
                std::make_shared<websocket_connection>(std::move(socket))->start();
            websocket_server(acceptor, socket);
        });
}