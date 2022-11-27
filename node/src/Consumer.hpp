#pragma once

#include "common/ConnectionHandler.hpp"
#include "common/NodeConfiguration.hpp"
#include <amqpcpp.h>
#include <ev.h>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "common/serialization/MessageSerializer.hpp"
#include "common/serialization/HeaderSerializer.hpp"
#include "Buffer.hpp"

class Consumer
{
public:
    Consumer(const common::NodeConfiguration& config, Buffer& buffer);
    Consumer(const Consumer&) = delete;
    Consumer(Consumer&&) = delete;
    Consumer& operator=(const Consumer&) = delete;
    Consumer& operator=(Consumer&&) = delete;
    ~Consumer();

    void publish(const common::itf::Block& block);
    void run();

private:
    struct ev_loop* loop;
    common::ConnectionHandler handler;
    AMQP::TcpConnection connection;
    AMQP::TcpChannel channel;
    Buffer& buffer;
    std::vector<std::string> clientNames{};
    serialization::HeaderSerializer headerSerializer{};
    serialization::MessageSerializer messageSerializer{};

    void handleControlMessage(const std::string& command);
    void createChannel(const std::string& command, const std::string& author);
    void sendDataToClients(
        const std::string& clientName, const std::vector<common::itf::Message>& data);
};
