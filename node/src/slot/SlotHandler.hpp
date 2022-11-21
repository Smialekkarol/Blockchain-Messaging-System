#pragma once

#include <functional>
#include <vector>

#include "db/RedisDB.hpp"
#include "node/src/Buffer.hpp"

namespace slot {
class SlotHandler {
public:
  SlotHandler(::db::RedisDB &redis_, Buffer &buffer_);

  void handle();

private:
  void savePendingBlock();
  void saveCompleteBlock();

  ::db::RedisDB &redis;
  Buffer &buffer;
  long long blockIndex{0};
  ::common::itf::Block block{};
  bool shouldCallNextHandler{true};
  std::vector<std::function<void()>> handlers{
      [this]() { savePendingBlock(); }, [this]() { saveCompleteBlock(); }};
};
} // namespace slot