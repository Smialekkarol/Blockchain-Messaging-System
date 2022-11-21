#pragma once

#include <vector>

#include <boost/serialization/vector.hpp>

#include "BlockState.hpp"
#include "Message.hpp"

namespace common::itf {
struct Block {
  Block();
  Block(const std::vector<Message> &data);
  Block(const uint64_t timestamp, const std::vector<Message> &data);

  bool operator==(const Block &other) const;

  BlockState state{BlockState::PENDING};
  uint64_t timestamp{0};
  uint64_t slot{0};
  std::vector<Message> data{};
};
} // namespace common::itf

namespace boost::serialization {
template <class Archive>
void serialize(Archive &archive, ::common::itf::Block &block,
               const unsigned int version) {
  archive &block.state;
  archive &block.timestamp;
  archive &block.slot;
  archive &block.data;
}
} // namespace boost::serialization
