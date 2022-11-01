#include "common/utils/Utils.hpp"

#include "Block.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace itf {
Block::Block() = default;
Block::Block(const std::vector<Message> &data)
    : timestamp{::common::utils::getTimeStamp()}, data{data} {}
Block::Block(const uint64_t timestamp, const std::vector<Message> &data)
    : timestamp{timestamp}, data{data} {}
} // namespace itf

