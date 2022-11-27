#pragma once

#include <iostream>
#include "common/ChannelStore.hpp"
#include "common/utils/Timer.hpp"
#include "common/serialization/MessageSerializer.hpp"
#include "common/serialization/HeaderSerializer.hpp"
#include "ClientInfo.hpp"
#include "AmqpHandler.hpp"

namespace client {

class ClientAMQP {
public:
  explicit ClientAMQP(ClientInfo &clientInfo);

  ~ClientAMQP();

  void makeInitialConnection(const std::string& channelName);

  void sendMessage(const std::string &queName, const std::string& message);

  void getData(const std::string& queName, const std::string &date);

  void listen(const std::string& que);

  std::string serializeData(common::itf::Header & header, common::itf::Message & message);
  
private:
  ClientInfo clientInfo;
  AmqpHandler amqpHandler;
  common::Buffer<std::string> buffer{};
  common::utils::Timer timer{};
  ChannelStore channelStore{};
  serialization::HeaderSerializer headerSerializer{};
  serialization::MessageSerializer messageSerializer{};
  std::mutex mutex{};
};

}