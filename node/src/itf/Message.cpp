#pragma once

#include "Message.hpp"

namespace itf {
Message::Message() = default;
Message::Message(const uint64_t timestamp_, const std::string &data_,
                 const std::string &author_)
    : timestamp{timestamp_}, data{data_}, author{author_} {}
} // namespace itf

namespace boost::serialization {
template <class Archive>
void serialize(Archive &archive, ::itf::Message &message,
               const unsigned int version) {
  archive &message.timestamp;
  archive &message.data;
  archive &message.author;
}
} // namespace boost::serialization
