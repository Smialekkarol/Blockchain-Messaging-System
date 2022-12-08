#include "ClientAMQP.hpp"
#include "common/utils/Time.hpp"
#include "common/utils/Timestamp.hpp"
#include "common/utils/Text.hpp"

namespace client
{

ClientAMQP::ClientAMQP(ClientInfo& clientInfo)
    : clientInfo(clientInfo)
    , amqpHandler("amqp://localhost/")
{
    channelStore.addChannel(ClientChannel(clientInfo.clientName));
    std::string queName{ clientInfo.clientName + "_ClientChannel" };
    amqpHandler.createQueue(queName);
    listen(queName);
}

ClientAMQP::~ClientAMQP()
{
}

void ClientAMQP::makeInitialConnection(const std::string& queToBeCreated)
{
    try
    {
        std::string queName = clientInfo.serverName + "_ControlChannel";

        common::itf::Header header(
            "CreateChannel", clientInfo.serverName, queName, clientInfo.clientName);
        common::itf::Message message(
            common::utils::Timestamp::get(), queToBeCreated, clientInfo.clientName);

        const std::string & data = serializeData(header, message);
        amqpHandler.send(queName, data);
    }
    catch (const std::exception& e)
    {
        spdlog::error(e.what());
    }
}

void ClientAMQP::sendMessage(const std::string& queName, const std::string& dataToSend)
{
    try
    {
        common::itf::Header header("Send", clientInfo.serverName, queName, clientInfo.clientName);
        common::itf::Message message(
            common::utils::Timestamp::get(), dataToSend, clientInfo.clientName);

        const std::string& data = serializeData(header, message);


        amqpHandler.send(queName, data);
    }
    catch (const std::exception& e)
    {
        spdlog::error(e.what());
    }
}

void ClientAMQP::getData(const std::string& queName, const std::string& RequestData)
{
    try
    {
        common::itf::Header header(
            "GetHistory", clientInfo.serverName, queName, clientInfo.clientName);
        common::itf::Message message(
            common::utils::Timestamp::get(), RequestData, clientInfo.clientName);

        const std::string& data = serializeData(header, message);
    }
    catch (const std::exception& e)
    {
        spdlog::error(e.what());
    }
}

void ClientAMQP::listen(const std::string& que)
{
    std::thread{
        [this, que]()
        {
            AmqpHandler("amqp://localhost/").receive(que, buffer).runEventLoop();
        }
    }.detach();

    auto handleBufferedData = [this]()
    {
        if (not buffer.isEmpty())
        {
            std::scoped_lock(mutex);
            for (const auto& msg : buffer.getData())
            {
                std::vector<std::string> data = common::utils::Text::splitBySeparator(msg, "@");
                const std::string& serializedMessage = data.at(0);
                auto message = messageSerializer.deserialize(serializedMessage);
                std::cout << common::utils::Time::getCurrentTime() << " " << message.author << ": "
                          << message.data << std::endl;
            }
            buffer.clearBuffer();
        }
    };
    timer.setInterval(handleBufferedData, 200);
}

std::string ClientAMQP::serializeData(common::itf::Header& header, common::itf::Message& message)
{
    return headerSerializer.serialize(header) + "@" + messageSerializer.serialize(message) + "\n";
}
}
