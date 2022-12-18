#pragma once

namespace client {

struct ClientInfo {
  ClientInfo(const std::string & host, const std::string& serverName, const std::string & clientName)
      : host(host), serverName(serverName), clientName(clientName) {}

  const std::string host;
  const std::string serverName;
  const std::string clientName;
};

}