#pragma once

#include <cstdint>

// TODO this class needs to be removed and Time class will be used instead
namespace common::utils {
class Timestamp {
public:
  static const uint64_t get();
};
} // namespace common::utils