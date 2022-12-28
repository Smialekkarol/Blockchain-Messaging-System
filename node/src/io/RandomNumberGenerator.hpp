#pragma once

#include <cstdint>
#include <random>

namespace io {
class RandomNumberGenerator {
public:
  std::uint64_t generate();

private:
  std::random_device seed{};
  std::mt19937_64 engine{seed()};
  std::uniform_int_distribution<std::uint64_t> generator{1, UINT16_MAX};
};
} // namespace io