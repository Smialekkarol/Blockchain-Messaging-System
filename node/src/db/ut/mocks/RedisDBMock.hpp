#pragma once

#include <static_mock/Mock.hpp>

#if !USE_ORIGINAL_CLASS(RedisDB)

#include "gmock/gmock.h"
#include <gtest/gtest.h>

#include "common/itf/Block.hpp"

namespace db {
class RedisDBMock {
public:
  MOCK_METHOD(long long, add,
              (const ::common::itf::Block &, const std::string &), ());
  MOCK_METHOD(void, update,
              (const ::common::itf::Block &, const long long,
               const std::string &),
              ());
  MOCK_METHOD(std::vector<::common::itf::Block>, get,
              (const std::string &, const int), ());
  MOCK_METHOD(void, save, (), ());
};

STATIC_MOCK_CLASS(RedisDBConstructor) {
public:
  MOCK_METHOD(std::shared_ptr<RedisDBMock>, construct, (), ());
  MOCK_METHOD(std::shared_ptr<RedisDBMock>, construct, (const std::string &), ());
};

class MOCK_FACADE(RedisDB) : public ::testing::MockBase<RedisDBMock> {
public:
  RedisDB() { this->setMock(STATIC_MOCK(RedisDBConstructor).construct()); }
  RedisDB(const std::string &redisServerAddress) {
    this->setMock(
        STATIC_MOCK(RedisDBConstructor).construct(redisServerAddress));
  }

  WRAP_METHOD(add);
  WRAP_METHOD(update);
  WRAP_METHOD(get);
  WRAP_METHOD(save);
};
} // namespace db
#endif
