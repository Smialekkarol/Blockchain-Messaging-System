#include "ClientAMQP.hpp"
#include <ctime>

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

int ClientAMQP::MakeInitialConnection(std::string queToBeCreated)
{
    try
    {
        std::string queName = clientInfo.serverName + "_ControlChannel";

        common::itf::Header header(
            "CreateChannel", clientInfo.serverName, queName, clientInfo.clientName);
        common::itf::Message message(
            common::utils::Timestamp::get(), queToBeCreated, clientInfo.clientName);

        serialization::MessageSerializer messageSerializer;
        serialization::HeaderSerializer headerSerializer;

        auto data = headerSerializer.serialize(header) + "@" + messageSerializer.serialize(message)
            + "@\n";

        amqpHandler.send(queName, data);
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}

int ClientAMQP::SendMessage(std::string queName, std::string dataToSend)
{
    try
    {
        serialization::MessageSerializer messageSerializer;
        serialization::HeaderSerializer headerSerializer;

        common::itf::Header header("Send", clientInfo.serverName, queName, clientInfo.clientName);
        common::itf::Message message(
            common::utils::Timestamp::get(), dataToSend, clientInfo.clientName);

        std::string data = headerSerializer.serialize(header) + "@"
            + messageSerializer.serialize(message) + "@\n";


        amqpHandler.send(queName, data);
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int ClientAMQP::getData(std::string queName, std::string RequestData)
{
    try
    {
        serialization::MessageSerializer messageSerializer;
        serialization::HeaderSerializer headerSerializer;

        common::itf::Header header(
            "GetHistory", clientInfo.serverName, queName, clientInfo.clientName);
        common::itf::Message message(
            common::utils::Timestamp::get(), RequestData, clientInfo.clientName);

        std::string data = headerSerializer.serialize(header);
        +"@" + messageSerializer.serialize(message) + "\n";
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}

void ClientAMQP::listen(std::string que)
{
    std::thread{
        [this, que]()
        {
            AmqpHandler("amqp://localhost/").receive(que, buffer).runEventLoop();
        }
    }.detach();

    auto print = [this]()
    {
        if (buffer.getSize() > 0)
        {
            serialization::HeaderSerializer headerSerializer;
            serialization::MessageSerializer messageSerializer;
            std::string serializedData;
            time_t now = time(0);
            tm* ltm = localtime(&now);

            for (auto msg : buffer.getBufferedData())
            {
                std::vector<std::string> dataAttributes
                    = common::utils::Text::splitBySeparator(msg, "@");
                auto message = dataAttributes[0];
                auto messageObject = messageSerializer.deserialize(message);
                std::cout << 1 + ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << " "
                          << messageObject.author << ": " << messageObject.data << std::endl;
            }
            buffer.clearBuffer();
        }
    };
    timer.setInterval(print, 200);
}
}
