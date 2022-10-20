#pragma once

#include <string>
#include <vector>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>

// TODO: make this path absolute once proper CMake will be created
#include "../common/utils/Utils.hpp"
#include "Message.hpp"

namespace itf {

struct Block {
  Block() = default;
  Block(const std::vector<Message> &data)
      : timestamp{::common::utils::getTimeStamp()}, data{data} {}
  Block(const uint64_t timestamp, const std::vector<Message> &data)
      : timestamp{timestamp}, data{data} {}
  Block(const Block &other) = default;
  Block(Block &&other) = default;
  Block &operator=(const Block &other) = default;
  Block &operator=(Block &&other) = default;
  ~Block() = default;

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
