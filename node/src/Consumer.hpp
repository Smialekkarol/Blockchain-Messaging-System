#pragma once

#include "common/ConnectionHandler.hpp"
#include "common/NodeConfiguration.hpp"
#include "common/Buffer.hpp"
#include <amqpcpp.h>
#include <ev.h>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "common/serialization/MessageSerializer.hpp"


//refactor klast consumer -> messageBroker
//2 implementacje consumera
//1 dla controlChannel
//2 dla messageChannela
//3 potencjalnie 3 dla mechanizmu konsensusu


class Consumer {
public:
    Consumer(const common::NodeConfiguration& config);
    Consumer(const Consumer&) = delete;
    Consumer(Consumer&&) = delete;
    Consumer& operator=(const Consumer&) = delete;
    Consumer& operator=(Consumer&&) = delete;
    ~Consumer();

    static void run(const common::NodeConfiguration& config);

private:
    struct ev_loop* loop;
    common::ConnectionHandler handler;
    AMQP::TcpConnection connection;
    AMQP::TcpChannel channel;
    common::Buffer<common::itf::Message> buffer{};
    std::vector<std::string> clientNames;

    void run();
    void handleControlMessage(const std::string& command);
    void createChannel(const std::string& command);
    void sendDataToClients(const std::string clientName, std::string data);
};
