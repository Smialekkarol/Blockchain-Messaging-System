#include <spdlog/spdlog.h>

#include "common/utils/Text.hpp"
#include "common/utils/Time.hpp"
#include "common/utils/Timestamp.hpp"

#include "Client.hpp"

namespace client {

Client::Client(ClientInfo &clientInfo)
    : clientInfo(clientInfo), amqpHandler(clientInfo.host) {
  std::string queName{clientInfo.clientName + "_ClientChannel"};
  amqpHandler.createQueue(queName);
  listen(queName);
}

Client::~Client() {}

void Client::createQueue(const std::string &queToBeCreated) {
  try {
    std::string queName = clientInfo.serverName + "_ControlChannel";

    common::itf::Header header("CreateChannel", clientInfo.serverName, queName,
                               clientInfo.clientName);
    common::itf::Message message(common::utils::Timestamp::get(),
                                 queToBeCreated, clientInfo.clientName);

    const std::string &data = serializeData(header, message);
    amqpHandler.sendMessage(queName, data);
  } catch (const std::exception &e) {
    spdlog::error(e.what());
  }
}

void Client::sendMessage(const std::string &queName,
                         const std::string &dataToSend) {
  try {
    common::itf::Header header("Send", clientInfo.serverName, queName,
                               clientInfo.clientName);
    common::itf::Message message(common::utils::Timestamp::get(), dataToSend,
                                 clientInfo.clientName);

    const std::string &data = serializeData(header, message);
    amqpHandler.sendMessage(queName, data);
  } catch (const std::exception &e) {
    spdlog::error(e.what());
  }
}

std::vector<common::itf::Message>
Client::getMessages(const std::string &queName,
                    const std::string &RequestData) {
  try {
    common::itf::Header header("getMessages", clientInfo.serverName, queName,
                               clientInfo.clientName);
    common::itf::Message message(common::utils::Timestamp::get(), RequestData,
                                 clientInfo.clientName);

    const std::string &data = serializeData(header, message);
  } catch (const std::exception &e) {
    spdlog::error(e.what());
  }
  return {};
}

void Client::listen(const std::string &queue) {
  std::thread{[this, queue]() {
    amqpHandler.receive(queue, buffer).runEventLoop();
  }}.detach();

  auto handleBufferedData = [this]() {
    if (not buffer.isEmpty()) {
      std::scoped_lock(mutex);
      for (const auto &msg : buffer.getData()) {
        std::vector<std::string> data =
            common::utils::Text::splitBySeparator(msg, "@");
        const std::string &serializedMessage = data.at(0);
        auto message = messageSerializer.deserialize(serializedMessage);
        spdlog::info("[{}] {}: {}", common::utils::Time::getCurrentTime(),
                     message.author, message.data);
        std::cout << common::utils::Time::getCurrentTime() << " "
                  << message.author << ": " << message.data << std::endl;
      }
      buffer.clearBuffer();
    }
  };
  timer.setInterval(handleBufferedData, 200);
}

std::string Client::serializeData(common::itf::Header &header,
                                  common::itf::Message &message) {
  return headerSerializer.serialize(header) + "@" +
         messageSerializer.serialize(message) + "\n";
}
} // namespace client
