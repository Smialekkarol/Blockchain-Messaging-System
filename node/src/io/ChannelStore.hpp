#pragma once

#include <memory>
#include <unordered_map>

#include "Channel.hpp"

namespace io {

class ChannelStore {
  using Address = std::string;

public:
  void pushLocal(const Address &address, std::unique_ptr<Channel> channel);
  void pushRemote(const Address &address, std::unique_ptr<Channel> channel);
  Channel &getLocal(const Address &address);
  std::unordered_map<Address, std::unique_ptr<Channel>> &getLocal();
  Channel &getRemote(const Address &address);
  std::unordered_map<Address, std::unique_ptr<Channel>> &getRemote();

private:
  std::unordered_map<Address, std::unique_ptr<Channel>> localChannels{};
  std::unordered_map<Address, std::unique_ptr<Channel>> remoteChannels{};
};
} // namespace io