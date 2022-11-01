
#include "Header.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace itf {
Header::Header() = default;
Header::Header(const std::string & target, const std::string & serverName,
          const std::string & queName)
    : target{target}, serverName{serverName}, queName{queName} {}
} // namespace itf

