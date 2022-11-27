#pragma once

// clang-format off
// #include <static_mock/Mock.hpp>
// #include INCLUDE_TESTABLE_MOCK("ut/mocks/TimeMock.hpp")
// #if USE_ORIGINAL_CLASS(Time)
// clang-format on

#include <cstdint>

// TODO this class needs to be tested
namespace common::utils {
class Time {
public:
  Time() = delete;

  static std::uint64_t getTimeStamp();
  static std::uint64_t getSlot();
  static std::uint64_t getSlot(const std::uint64_t timestamp);
  static std::uint16_t getTimeToTheNearestSlot();
  static std::uint16_t getTimeToTheNearestSlot(const std::uint64_t timestamp);

  static constexpr std::uint16_t SLOT{500};
};
} // namespace common::utils
// #endif
