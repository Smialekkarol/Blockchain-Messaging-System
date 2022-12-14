#pragma once

#include <functional>
#include <vector>

#include "db/RedisDB.hpp"
#include "node/src/Buffer.hpp"
#include "node/src/Consumer.hpp"
#include "node/src/io/ChannelStore.hpp"

namespace slot {
class SlotHandler {
public:
  SlotHandler(::db::RedisDB &redis_, Buffer &buffer_, Consumer &consumer,
              ::io::ChannelStore &channelStore_);

  void handle();

private:
  void savePendingBlock();
  void waitForNodesInspection();
  void saveCompleteBlock();
  void publishBlock();

  ::db::RedisDB &redis;
  Buffer &buffer;
  Consumer &consumer;
  ::io::ChannelStore &channelStore;
  long long blockIndex{0};
  ::common::itf::Block block{};
  bool shouldCallNextHandler{true};
  std::vector<std::function<void()>> handlers{
      [this]() { savePendingBlock(); }, [this]() { waitForNodesInspection(); },
      [this]() { saveCompleteBlock(); }, [this]() { publishBlock(); }};
};
} // namespace slot