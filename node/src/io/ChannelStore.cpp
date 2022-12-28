#include <algorithm>

#include "ChannelStore.hpp"

namespace io {

void ChannelStore::pushLocal(const ChannelStore::Address &address,
                             std::unique_ptr<Channel> channel) {
  std::scoped_lock lock(mutex);
  localChannels.emplace(std::make_pair(address, std::move(channel)));
}

void ChannelStore::pushRemote(const ChannelStore::Address &address,
                              std::unique_ptr<Channel> channel) {
  std::scoped_lock lock(mutex);
  remoteChannels.emplace(std::make_pair(address, std::move(channel)));
}

Channel &ChannelStore::getLocal(const ChannelStore::Address &address) {
  std::scoped_lock lock(mutex);
  return *localChannels.at(address);
}

std::unordered_map<ChannelStore::Address, std::unique_ptr<Channel>> &
ChannelStore::getLocal() {
  std::scoped_lock lock(mutex);
  return localChannels;
}

Channel &ChannelStore::getRemote(const ChannelStore::Address &address) {
  std::scoped_lock lock(mutex);
  return *remoteChannels.at(address);
}

std::vector<Channel *> ChannelStore::getRemote(const ChannelType channelType) {
  std::scoped_lock lock(mutex);
  std::vector<Channel *> result{};
  for (const auto &[k, v] : remoteChannels) {
    if (v->type != channelType) {
      continue;
    }
    result.push_back(v.get());
  }
  return result;
}

std::unordered_map<ChannelStore::Address, std::unique_ptr<Channel>> &
ChannelStore::getRemote() {
  std::scoped_lock lock(mutex);
  return remoteChannels;
}

} // namespace io