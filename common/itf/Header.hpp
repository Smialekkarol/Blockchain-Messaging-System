#pragma once

#include <string>

namespace common::itf {

struct Header {
  Header();
  Header(const std::string &target, const std::string &serverName,
         const std::string &queName);

  std::string target{};
  std::string serverName{};
  std::string queName{};
};

} // namespace common::itf

namespace boost::serialization {
template <class Archive>
void serialize(Archive &archive, ::common::itf::Header &header,
               const unsigned int version) {
  archive &header.target;
  archive &header.serverName;
  archive &header.queName;
}
} // namespace boost::serialization
