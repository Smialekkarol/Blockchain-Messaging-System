#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include "common/utils/Timestamp.hpp"
#include "common/utils/Text.hpp"
#include "common/utils/Timer.hpp"
#include "common/serialization/MessageSerializer.hpp"
#include "common/serialization/HeaderSerializer.hpp"

#include "ClientInfo.hpp"

#include "AmqpHandler.hpp"
#include "common/ChannelStore.hpp"

namespace client {

class ClientAMQP {
public:
  ClientAMQP(ClientInfo &clientInfo);

  ~ClientAMQP();

  int MakeInitialConnection(std::string channelName);

  int SendMessage(std::string queName, std::string message);

  int getData(std::string queName, std::string date);

  void listen(std::string que);
  
private:
  ClientInfo clientInfo;
  common::utils::Timer timer{};
  AmqpHandler amqpHandler;
  common::Buffer<std::string> buffer{};
  ChannelStore channelStore{};

};
}