#pragma once

namespace client {

struct ClientInfo {
  ClientInfo(const std::string & host, const std::string & port, const std::string& serverName, const std::string & clientName)
      : host(host), port(port), serverName(serverName), clientName(clientName) {}

  const std::string host;
  const std::string port;
  const std::string serverName;
  const std::string clientName;
};

}