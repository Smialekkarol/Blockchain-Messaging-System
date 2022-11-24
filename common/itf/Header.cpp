
#include "Header.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace common::itf {
Header::Header() = default;
Header::Header(const std::string &target, const std::string &serverName,
               const std::string &queName, const std::string & clientAddress)
    : target{target}, serverName{serverName}, queName{queName}, clientAddress{clientAddress} {}
} // namespace common::itf
