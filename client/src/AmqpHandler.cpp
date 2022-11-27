#include "AmqpHandler.hpp"

namespace client
{
AmqpHandler::AmqpHandler(std::string address)
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
    ev_loop_destroy(loop);
}

void AmqpHandler::createQueue(std::string que)
{
    channel.declareQueue(que, AMQP::durable)
        .onSuccess([this](const std::string& name, uint32_t messagecount, uint32_t consumercount)
            {
                std::cout << "que " + name + " created\n";
                ev_break(loop, EVBREAK_ONE);
            })
        .onError([](const char* message)
            {
                spdlog::error(message);
            });
    ev_run(loop, 0);
}

void AmqpHandler::send(std::string que, std::string message)
{
    struct ev_loop* loop = ev_loop_new();
    channel.publish("", que, message);
    ev_run(loop, 0);
}

AmqpHandler& AmqpHandler::receive(std::string que, common::Buffer<std::string>& buffer)
{
    std::string message{};
    struct ev_loop* loop = ev_loop_new();
    channel.consume(que, AMQP::noack)
        .onReceived([this, &buffer](const AMQP::Message& msg, uint64_t tag, bool redelivered)
            {   
                buffer.addMessage(msg.body());
        
            })
        .onSuccess([](const std::string& consumertag)
            {

                std::cout << "consume operation started: " << consumertag << std::endl;
            })
        .onError([](const char* message)
            {

                std::cout << "consume operation failed : " << message << std::endl;
            });
    return *this;
}
void AmqpHandler::runEventLoop()
{
    ev_run(loop, 0);
}
}