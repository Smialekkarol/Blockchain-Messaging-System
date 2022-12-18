#include "AmqpHandler.hpp"

namespace client
{
AmqpHandler::AmqpHandler(const std::string & address)
    : loop(ev_loop_new(0))
    , handler(loop)
    , connection(&handler, AMQP::Address(address))
    , channel(&connection)
{
    channel.onError([](const char* message)
        {
            spdlog::error("Channel error: {}", message);
        });
}

AmqpHandler::~AmqpHandler()
{
    connection.close();
    ev_loop_destroy(loop);
}

void AmqpHandler::createQueue(const std::string & queue)
{
    channel.declareQueue(queue, AMQP::durable)
        .onSuccess([this](const std::string& name, uint32_t messagecount, uint32_t consumercount)
            {
                spdlog::debug("queue " + name + " created");
                ev_break(loop, EVBREAK_ONE);
            })
        .onError([](const char* message)
            {
                spdlog::error(message);
            });
    ev_run(loop, 0);
}

void AmqpHandler::sendMessage(const std::string & queue, const std::string & message)
{
    struct ev_loop* loop = ev_loop_new();
    channel.publish("", queue, message);
    ev_run(loop, 0);
}

AmqpHandler& AmqpHandler::receive(const std::string & queue, common::Buffer<std::string>& buffer)
{
    std::string message{};
    struct ev_loop* loop = ev_loop_new();
    channel.consume(queue, AMQP::noack)
        .onReceived([this, &buffer](const AMQP::Message& msg, uint64_t tag, bool redelivered)
            {
                buffer.pushBack(msg.body());
            })
        .onSuccess([](const std::string& consumertag)
            {
                spdlog::debug("consume operation started: " + consumertag);
            })
        .onError([](const std::string& message)
            {
                spdlog::debug("consume operation failed : " + message);
            });
            
    return *this;
}
void AmqpHandler::runEventLoop()
{
    ev_run(loop, 0);
}
}