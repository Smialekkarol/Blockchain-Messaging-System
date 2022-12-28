#pragma once

#include <cstdint>
#include <string>

#include "ConsensusOperation.hpp"

namespace io {

struct Header {
  ConsensusOperation operation{ConsensusOperation::INSPECTION};
  std::string node{};
  std::string address{};
  uint64_t timestamp{0};
  uint64_t slot{0};
};

} // namespace io

namespace boost::serialization {
template <class Archive>
void serialize(Archive &archive, ::io::Header &header,
               const unsigned int version) {
  archive &header.operation;
  archive &header.address;
  archive &header.node;
  archive &header.timestamp;
  archive &header.slot;
}
} // namespace boost::serialization
