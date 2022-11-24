#pragma once

#include <boost/beast/core/buffers_to_string.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include "common/utils/Timestamp.hpp"
#include "common/utils/Text.hpp"
#include "common/serialization/MessageSerializer.hpp"
#include "common/serialization/HeaderSerializer.hpp"
#include "common/utils/Timer.hpp"
#include "ClientInfo.hpp"
#include "WebsocketHandler.hpp"

namespace client {

class Client {
public:
  Client(net::io_context &ioc, ClientInfo &clientInfo);

  ~Client();

  int MakeInitialConnection(std::string channelName);

  int SendMessage(std::string queName, std::string message);

  int getData(std::string queName, std::string date);

  void Listen();
  
private:
  ClientInfo clientInfo;
  WebsocketHandler websocketHandler;
};
}