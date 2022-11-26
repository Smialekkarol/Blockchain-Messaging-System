#pragma once
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <ev.h>
#include "common/ConnectionHandler.hpp"
#include "common/serialization/MessageSerializer.hpp"
#include "common/Buffer.hpp"

namespace client
{
using namespace common;

class AmqpHandler
{
public:
    AmqpHandler(std::string address);
    ~AmqpHandler();
    void createQueue(std::string que);
    void send(std::string que, std::string message);
    AmqpHandler& receive(std::string que, common::Buffer<std::string>& buffer);
    void runEventLoop();

private:
    struct ev_loop* loop;
    common::ConnectionHandler handler;
    AMQP::TcpConnection connection;
    AMQP::TcpChannel channel;
};
}