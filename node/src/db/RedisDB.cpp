#include "RedisDB.hpp"

#include <algorithm>

namespace db {
RedisDB::RedisDB() : redis{"tcp://127.0.0.1:6379"} {}

RedisDB::RedisDB(const std::string &redisServerAddress)
    : redis{redisServerAddress} {}

long long RedisDB::add(const ::common::itf::Block &block,
                       const std::string &blockchain) {
  std::scoped_lock lock(mutex);
  std::vector<std::string> serializedBlock = {blockSerializer.serialize(block)};
  return redis.rpush(blockchain, serializedBlock.begin(),
                     serializedBlock.end());
}

void RedisDB::update(const ::common::itf::Block &block, const long long index,
                     const std::string &blockchain) {
  std::scoped_lock lock(mutex);
  std::string serializedBlock = {blockSerializer.serialize(block)};
  redis.lset(blockchain, index, serializedBlock);
}

void RedisDB::remove(const ::common::itf::Block &block,
                     const std::string &blockchain) {
  std::scoped_lock lock(mutex);
  std::string serializedBlock{blockSerializer.serialize(block)};
  redis.lrem(blockchain, 1, serializedBlock);
}

std::optional<::common::itf::Block>
RedisDB::getByIndex(const int index, const std::string &blockchain) {
  std::scoped_lock lock(mutex);
  std::vector<std::string> serializedBlocks{};
  redis.lrange(blockchain, index, index, std::back_inserter(serializedBlocks));
  if (serializedBlocks.size() == 0) {
    return {};
  }
  const auto &deserializedBlock =
      blockSerializer.deserialize(serializedBlocks.at(0));
  return deserializedBlock;
}

std::vector<::common::itf::Block> RedisDB::get(const std::string &blockchain,
                                               const int numberOfBlocks) {
  std::scoped_lock lock(mutex);
  std::vector<std::string> serializedBlocks{};
  redis.lrange(blockchain, -1 * numberOfBlocks, -1,
               std::back_inserter(serializedBlocks));
  return blockSerializer.deserialize(serializedBlocks);
}

void RedisDB::save() { redis.save(); }
} // namespace db
