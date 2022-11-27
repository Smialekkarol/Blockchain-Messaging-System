#pragma once

#include <functional>
#include <vector>

#include "db/RedisDB.hpp"
#include "node/src/Buffer.hpp"
#include "node/src/Consumer.hpp"

namespace slot {
class SlotHandler {
public:
  SlotHandler(::db::RedisDB &redis_, Buffer &buffer_, Consumer &consumer);

  void handle();

private:
  void savePendingBlock();
  void saveCompleteBlock();
  void publishBlock();

  ::db::RedisDB &redis;
  Buffer &buffer;
  Consumer &consumer;
  long long blockIndex{0};
  ::common::itf::Block block{};
  bool shouldCallNextHandler{true};
  std::vector<std::function<void()>> handlers{
      [this]() { savePendingBlock(); }, [this]() { saveCompleteBlock(); }, [this]() { publishBlock(); }};
};
} // namespace slot