#include "Time.hpp"

#include <chrono>

namespace common::utils {
std::uint64_t Time::getTimeStamp() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}
std::uint64_t Time::getSlot() { return getTimeStamp() / SLOT; }

std::uint64_t Time::getSlot(const std::uint64_t timestamp) {
  return timestamp / SLOT;
}

std::uint16_t Time::getTimeToTheNearestSlot() {
  return SLOT - (getTimeStamp() % SLOT);
}

std::uint16_t Time::getTimeToTheNearestSlot(const std::uint64_t timestamp) {
  return SLOT - (timestamp % SLOT);
}
} // namespace common::utils