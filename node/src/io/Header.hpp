#pragma once

#include <cstdint>
#include <string>

namespace io {

enum class Operation : std::uint16_t { ELECTION, UPLOADING, BROADCAST };

struct Header {
  Operation operation{Operation::ELECTION};
  std::string node{};
  uint64_t timestamp{0};
  uint64_t slot{0};
};

} // namespace io

namespace boost::serialization {
template <class Archive>
void serialize(Archive &archive, ::io::Header &header,
               const unsigned int version) {
  archive &header.operation;
  archive &header.node;
  archive &header.timestamp;
  archive &header.slot;
}
} // namespace boost::serialization
