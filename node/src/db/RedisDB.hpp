#pragma once

// clang-format off
#include <static_mock/Mock.hpp>
#include INCLUDE_TESTABLE_MOCK("ut/mocks/RedisDBMock.hpp")
#if USE_ORIGINAL_CLASS(RedisDB)
// clang-format on

#include <mutex>

#include <sw/redis++/redis++.h>

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
#endif
