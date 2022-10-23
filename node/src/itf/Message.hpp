#pragma once

#include <string>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace itf {

struct Message {
  Message();
  Message(const uint64_t timestamp_, const std::string &data_,
          const std::string &author_);

  uint64_t timestamp{0};
  std::string data{};
  std::string author{};
};

} // namespace itf

namespace boost::serialization {
template <class Archive>
void serialize(Archive &archive, ::itf::Message &message,
               const unsigned int version);
} // namespace boost::serialization
