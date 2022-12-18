#pragma once

#include <iostream>
#include "common/utils/Timer.hpp"
#include "common/serialization/MessageSerializer.hpp"
#include "common/serialization/HeaderSerializer.hpp"
#include "ClientInfo.hpp"
#include "AmqpHandler.hpp"

namespace client
{

class Client
{
public:
    explicit Client(ClientInfo& clientInfo);

    ~Client();

    void createQueue(const std::string& queueName);

    void sendMessage(const std::string& queueName, const std::string& message);

    std::vector<common::itf::Message> getMessages(
        const std::string& queueName, const std::string& date);

    void listen(const std::string& queue);

private:

    std::string serializeData(common::itf::Header& header, common::itf::Message& message);

    ClientInfo clientInfo;
    AmqpHandler amqpHandler;
    common::Buffer<std::string> buffer{};
    common::utils::Timer timer{};
    serialization::HeaderSerializer headerSerializer{};
    serialization::MessageSerializer messageSerializer{};
    std::mutex mutex{};
};
}