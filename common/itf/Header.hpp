#pragma once

#include <string>

namespace common::itf {

struct Header {
  Header();
  Header(const std::string &target, const std::string &serverName,
         const std::string &queName, const std::string &clientAddress);

  std::string target{};
  std::string serverName{};
  std::string queName{};
  std::string clientAddress{};
};

} // namespace common::itf

namespace boost::serialization {
template <class Archive>
void serialize(Archive &archive, ::common::itf::Header &header,
               const unsigned int version) {
  archive &header.target;
  archive &header.serverName;
  archive &header.queName;
  archive &header.clientAddress;
}
} // namespace boost::serialization
