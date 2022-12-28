#pragma once

#include <cstdint>

namespace io {
enum class ConsensusOperation : std::uint16_t {
  INSPECTION,
  ELECTION,
  UPLOADING,
  BROADCAST
};
}
