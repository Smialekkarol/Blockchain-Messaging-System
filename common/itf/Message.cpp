
#include "Message.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace common::itf {
Message::Message() = default;
Message::Message(const uint64_t timestamp_, const std::string &data_,
                 const std::string &author_)
    : timestamp{timestamp_}, data{data_}, author{author_} {}
} // namespace common::itf
