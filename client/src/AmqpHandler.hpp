#pragma once
#include "common/ConnectionHandler.hpp"
#include "common/Buffer.hpp"

namespace client
{

class AmqpHandler
{
public:
    AmqpHandler(const std::string & address);
    ~AmqpHandler();
    void createQueue(const std::string & queue);
    void sendMessage(const std::string & queue, const std::string & message);
    void runEventLoop();
    AmqpHandler& receive(const std::string & queue, common::Buffer<std::string>& buffer);

private:
    struct ev_loop* loop;
    common::ConnectionHandler handler;
    AMQP::TcpConnection connection;
    AMQP::TcpChannel channel;
};

}