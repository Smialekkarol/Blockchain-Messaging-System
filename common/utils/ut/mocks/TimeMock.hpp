#pragma once

#include <static_mock/Mock.hpp>

#if !USE_ORIGINAL_CLASS(Time)

#include "gmock/gmock.h"
#include <gtest/gtest.h>

namespace common::utils {
class TimeMock {
public:
  MOCK_METHOD(std::uint64_t, getTimeStamp, (), ());
  MOCK_METHOD(std::uint64_t, getSlot, (), ());
  MOCK_METHOD(std::uint64_t, getSlot, (const std::uint64_t), ());
  MOCK_METHOD(std::uint16_t, getTimeToTheNearestSlot, (), ());
  MOCK_METHOD(std::uint16_t, getTimeToTheNearestSlot, (const std::uint64_t),
              ());
};

class MOCK_FACADE(Time) : public ::testing::MockBase<TimeMock> {
public:
  Time() = delete;

  WRAP_METHOD_STATIC(TimeMock, getTimeStamp);
  WRAP_METHOD_STATIC(TimeMock, getSlot);
  WRAP_METHOD_STATIC(TimeMock, getTimeToTheNearestSlot);
};
} // namespace common::utils

#endif
