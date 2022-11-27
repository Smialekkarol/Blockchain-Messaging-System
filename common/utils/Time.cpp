#include "Time.hpp"
#include <chrono>
#include <ctime>

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

std::string Time::getCurrentTime(){
  time_t now = time(0);
  tm* localTime = localtime(&now);
  return std::to_string(1 + localTime->tm_hour) + ":" + std::to_string(localTime->tm_min);
}
} // namespace common::utils