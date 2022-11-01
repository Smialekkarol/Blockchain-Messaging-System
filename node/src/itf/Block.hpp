#pragma once

#include <vector>

#include <boost/serialization/vector.hpp>

#include "Message.hpp"

namespace itf {
struct Block {
  Block();
  Block(const std::vector<Message> &data);
  Block(const uint64_t timestamp, const std::vector<Message> &data);

  uint64_t timestamp{0};
  std::vector<Message> data{};
};
} // namespace itf

namespace boost::serialization {
template <class Archive>
void serialize(Archive &archive, ::itf::Block &block,
               const unsigned int version) {
  archive &block.timestamp;
  archive &block.data;
}
} // namespace boost::serialization
