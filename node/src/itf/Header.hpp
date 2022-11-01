#pragma once

#include <string>

namespace itf {

struct Header {
  Header();
  Header(const std::string & target, const std::string & serverName,
          const std::string & queName);

  std::string target{};
  std::string serverName{};
  std::string queName{};
};

} // namespace itf

namespace boost::serialization {
template <class Archive>
void serialize(Archive &archive, ::itf::Header &header,
               const unsigned int version){
  archive &header.target;
  archive &header.serverName;
  archive &header.queName;
}
} // namespace boost::serialization
