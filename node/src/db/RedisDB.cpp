#include <algorithm>

#include "RedisDB.hpp"

namespace db {
RedisDB::RedisDB() : redis{"tcp://127.0.0.1:6379"} {}

RedisDB::RedisDB(const std::string &redisServerAddress)
    : redis{redisServerAddress} {}

void RedisDB::add(const ::itf::Block &block, const std::string &blockchain) {
  std::vector<std::string> serializedBlock = {blockSerializer.serialize(block)};
  redis.rpush(blockchain, serializedBlock.begin(), serializedBlock.end());
  redis.save();
}

std::vector<::itf::Block> RedisDB::get(const std::string &blockchain,
                                       const int numberOfBlocks = 1) {
  std::vector<std::string> serializedBlocks{};
  redis.lrange(blockchain, -1 * numberOfBlocks, -1,
               std::back_inserter(serializedBlocks));
  return blockSerializer.deserialize(serializedBlocks);
}
} // namespace db
