#pragma once

#include <sstream>

#include <sw/redis++/redis++.h>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include "itf/Block.hpp"

#include "serialization/BlockSerializer.hpp"

namespace db {
class RedisDB {
public:
  RedisDB();
  RedisDB(const std::string &redisServerAddress);

  void add(const ::itf::Block &block, const std::string &blockchain);

  std::vector<::itf::Block> get(const std::string &blockchain,
                                const int numberOfBlocks = 1);

private:
  sw::redis::Redis redis;
  ::serialization::BlockSerializer blockSerializer{};
};
} // namespace db
