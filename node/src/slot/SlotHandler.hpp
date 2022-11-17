#pragma once

#include "db/RedisDB.hpp"
#include "node/src/Buffer.hpp"

namespace slot {
class SlotHandler {
public:
  SlotHandler(::db::RedisDB &redis_, Buffer &buffer_)
      : redis{redis_}, buffer{buffer_} {}

  void handle() {
    savePendingBlock();
    saveCompleteBlock();
  }

private:
  void savePendingBlock() {
    buffer.save();
    block = buffer.pop();
    blockIndex = redis.add(block, "default");
  };

  void saveCompleteBlock() {
    block.state = ::common::itf::BlockState::COMPLETED;
    redis.update(block, blockIndex, "default");
  }

  ::db::RedisDB &redis;
  Buffer &buffer;
  long long blockIndex{0};
  ::common::itf::Block block{};
};
} // namespace slot