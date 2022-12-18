#include "ChannelStore.hpp"

namespace io {

void ChannelStore::pushLocal(const ChannelStore::Address &address,
                             std::unique_ptr<Channel> channel) {
  localChannels.emplace(std::make_pair(address, std::move(channel)));
}

void ChannelStore::pushRemote(const ChannelStore::Address &address,
                              std::unique_ptr<Channel> channel) {
  remoteChannels.emplace(std::make_pair(address, std::move(channel)));
}

Channel &ChannelStore::getLocal(const ChannelStore::Address &address) {
  return *localChannels.at(address);
}

std::unordered_map<ChannelStore::Address, std::unique_ptr<Channel>> &
ChannelStore::getLocal() {
  return localChannels;
}

Channel &ChannelStore::getRemote(const ChannelStore::Address &address) {
  return *remoteChannels.at(address);
}

std::unordered_map<ChannelStore::Address, std::unique_ptr<Channel>> &
ChannelStore::getRemote() {
  return remoteChannels;
}

} // namespace io