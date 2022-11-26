#pragma once

#include <cstdint>

namespace common::itf {
enum class BlockState : std::uint16_t { PENDING = 0, COMPLETED = 1 };
}
