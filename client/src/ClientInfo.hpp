#pragma once

namespace client {

struct ClientInfo {
  ClientInfo(const std::string host, const char *port, std::string serverName, std::string clientName
             )
      : host(host), port(port), serverName(serverName), clientName(clientName) {}

  const std::string host;
  const char *port;
  std::string serverName;
  std::string clientName;
};
}