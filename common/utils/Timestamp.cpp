#include "Timestamp.hpp"
#include <chrono>

namespace common::utils {
const uint64_t Timestamp::get() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}
} // namespace common::utils