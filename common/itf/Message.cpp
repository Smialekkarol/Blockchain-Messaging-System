#include "Message.hpp"

namespace common::itf {
Message::Message() = default;

Message::Message(const uint64_t timestamp_, const std::string &data_,
                 const std::string &author_)
    : timestamp{timestamp_}, data{data_}, author{author_} {}

bool Message::operator==(const Message &other) const {
  return other.timestamp == timestamp && other.data == data &&
         other.author == author;
}
} // namespace common::itf
