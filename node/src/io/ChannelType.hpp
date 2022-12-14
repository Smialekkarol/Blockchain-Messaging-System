#pragma once

#include <cstdint>

namespace io {
enum class ChannelType : std::uint16_t {
  NODE,
  CLIENT,
  BLOCKCHAIN,
  CONTROL,
  CONSENSUS
};
}