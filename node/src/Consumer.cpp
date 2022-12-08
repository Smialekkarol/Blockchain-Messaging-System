#include <spdlog/spdlog.h>
#include <thread>
#include "Consumer.hpp"
#include "common/utils/Text.hpp"
#include "common/utils/Timer.hpp"


Consumer::Consumer(const common::NodeConfiguration& config, Buffer& buffer)
    : loop(ev_loop_new(0))
    , handler(loop)
    , connection(&handler, AMQP::Address(config.self.address))
    , channel(&connection)
    , buffer(buffer)
{
    channel.onError([](const char* message)
        {
            spdlog::error("Channel error: {}", message);
        });
    channel.declareQueue(config.self.name + "_ControlChannel", AMQP::durable)
        .onError([](const char* message)
            {
                spdlog::error(message);
            });
    channel.consume(config.self.name + "_ControlChannel", AMQP::noack)
        .onReceived([&](const AMQP::Message& msg, uint64_t tag, bool redelivered)
            {
                std::string message{ msg.body(), msg.body() + msg.bodySize() };
                spdlog::debug(config.self.name + "_ControlChannel" + " Received: {}", message);
                handleControlMessage(message);
            });
}

Consumer::~Consumer()
{
    ev_loop_destroy(loop);
}

void Consumer::run()
{
    ev_run(loop, 0);
}

void Consumer::handleControlMessage(const std::string& data)
{

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> at("@");
    tokenizer dataTokens(data, at);

    std::vector<std::string> dataAttributes;
    BOOST_FOREACH (std::string const& token, dataTokens)
    {
        dataAttributes.push_back(token);
    }

    const std::string& serializedHeader = dataAttributes.at(0);
    const std::string& serializedMessage = dataAttributes.at(1);

    const common::itf::Header& header = headerSerializer.deserialize(serializedHeader);
    const common::itf::Message& message = messageSerializer.deserialize(serializedMessage);


    if (header.target == "CreateChannel")
    {
        createChannel(message.data, message.author);
    }
}

void Consumer::createChannel(const std::string& channelToBeCreated, const std::string& author)
{

    channel.declareQueue(channelToBeCreated, AMQP::durable)
        .onError([](const char* message)
            {
                spdlog::error(message);
            })
        .onSuccess([this, author]()
            {
                if (std::find(clientNames.begin(), clientNames.end(), author) == clientNames.end())
                {
                    clientNames.push_back(author);
                }
            });


    channel.consume(channelToBeCreated, AMQP::noack)
        .onReceived([this](const AMQP::Message& msg, uint64_t tag, bool redelivered)
            {
                std::vector<std::string> data
                    = common::utils::Text::splitBySeparator(msg.body(), "@");

                const std::string serializedMessage = data.at(1);
                common::itf::Message message = messageSerializer.deserialize(serializedMessage);
                spdlog::debug(message.author + ": " + message.data + "\n");
                buffer.push(message);
            });
}

void Consumer::publish(const common::itf::Block& block)
{
    for (const auto& clientName : clientNames)
    {
        std::string clientChannel = clientName + "_ClientChannel";
        sendDataToClients(clientChannel, block.data);
    }
}

void Consumer::sendDataToClients(
    const std::string& clientChannel, const std::vector<common::itf::Message>& messages)
{
    for (const auto& msg : messages)
    {
        spdlog::debug("sent to " + clientChannel + "\n" + msg.data);
        const std::string data = messageSerializer.serialize(msg);
        channel.publish("", clientChannel, data);
    }
}