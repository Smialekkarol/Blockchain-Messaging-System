#pragma once

#include <mutex>
#include <sstream>

#include <sw/redis++/redis++.h>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include "common/itf/Block.hpp"

#include "serialization/BlockSerializer.hpp"

namespace db {
class RedisDB {
public:
  RedisDB();
  RedisDB(const std::string &redisServerAddress);

  long long add(const ::common::itf::Block &block,
                const std::string &blockchain);
  void update(const ::common::itf::Block &block, const long long index,
              const std::string &blockchain);

  std::vector<::common::itf::Block> get(const std::string &blockchain,
                                        const int numberOfBlocks = 1);

  void save();

private:
  sw::redis::Redis redis;
  ::serialization::BlockSerializer blockSerializer{};
  std::mutex mutex{};
};
} // namespace db
