#include "WebSocketServer.hpp"
#include <string.h>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

#include "/root/node/src/NodeConfiguration.hpp"
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <ev.h>
#include "/root/node/src/ConnectionHandler.hpp"

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

void websocket_connection::proccesRequestData(std::string message) const
{
    boost::char_separator<char> sep(";");
    tokenizer tokens(message, sep);
    std::vector<std::string> attributes;

    BOOST_FOREACH (std::string const& token, tokens) {
        attributes.push_back(token);
    }

    auto target = attributes[0];
    auto nodeName = attributes[1];
    

    if (target == "CreateChannel") {
        auto controlQueName = attributes[1] + "_ControlChannel";
        auto data = attributes[2];
        this->CreateNewQue("amqp://localhost/", nodeName, controlQueName, data);
        std::cout<<"CreateChannel request received\n";
    }
    if(target == "Send"){
        auto queName = attributes[2];
        auto data = attributes[3];
        std::cout<<"Send request received\n"<<nodeName;
        this->SendMessage(nodeName, queName, data);
    }
    if(target == "GetData"){
        auto queName = attributes[2];
        auto data = attributes[3];
        std::cout<<"GetData request received\n";
    }
}

void websocket_connection::CreateNewQue(std::string nodeAddress, std::string nodeName, std::string controlQueName, std::string queToBeCreated) const
{
    const std::string& message{queToBeCreated};
    NodeInfo node{ nodeAddress, nodeName };

    auto* loop = ev_loop_new(0);
    ConnectionHandler handler(loop);

    AMQP::TcpConnection connection(&handler, AMQP::Address(nodeAddress));
    AMQP::TcpChannel channel(&connection);
    channel.onError([](const char* message) {
        std::cout << "Channel error: " << message << std::endl;
    });
    std::cout<<"message sent on "<<controlQueName<<std::endl;
    channel.declareQueue(controlQueName, AMQP::durable)
        .onSuccess([&connection, &channel, &node,
            &message](const std::string& name, uint32_t messagecount,
            uint32_t consumercount) {
            channel.publish("", name, message);
            
        })
        .onError([](const char* message) { std::cout << message << "\n"; })
        .onFinalize([&connection]() { connection.close(); });
    ev_run(loop, 0);
}

void websocket_connection::SendMessage(std::string nodeName, std::string queName, std::string message) const{

    NodeInfo node{ "amqp://localhost/", nodeName };
    auto* loop = ev_loop_new(0);
    ConnectionHandler handler(loop);

    AMQP::TcpConnection connection(&handler, AMQP::Address("amqp://localhost/"));
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