#pragma once

#include <string>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace itf {

struct Message {
  Message() = default;
  Message(const uint64_t timestamp_, const std::string &data_)
      : timestamp{timestamp_}, data{data_} {}
  Message(const Message &other) = default;
  Message(Message &&other) = default;
  Message &operator=(const Message &other) = default;
  Message &operator=(Message &&other) = default;
  ~Message() = default;

  uint64_t timestamp{0};
  std::string data{};
};

} // namespace itf

namespace boost::serialization {
template <class Archive>
void serialize(Archive &archive, ::itf::Message &message,
               const unsigned int version) {
  archive &message.timestamp;
  archive &message.data;
}
} // namespace boost::serialization
