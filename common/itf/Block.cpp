#include "utils/Time.hpp"

#include "Block.hpp"

namespace common::itf {
Block::Block()
    : timestamp{::common::utils::Time::getTimeStamp()},
      slot{::common::utils::Time::getSlot(timestamp)} {}

Block::Block(const std::vector<Message> &data_)
    : timestamp{::common::utils::Time::getTimeStamp()},
      slot{::common::utils::Time::getSlot(timestamp)}, data{data_} {}

Block::Block(const uint64_t timestamp_, const std::vector<Message> &data_)
    : timestamp{timestamp_}, slot{::common::utils::Time::getSlot(timestamp)},
      data{data_} {}

bool Block::operator==(const Block &other) const {
  return other.timestamp == timestamp && other.slot == slot &&
         other.state == state && other.data == data;
}
} // namespace common::itf
