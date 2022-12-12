#pragma once

#include <cstdint>

namespace io {
struct ElectionValueWrapper {
  std::uint64_t value{0};
};
} // namespace io

namespace boost::serialization {
template <class Archive>
void serialize(Archive &archive,
               ::io::ElectionValueWrapper &electionValueWrapper,
               const unsigned int version) {
  archive &electionValueWrapper.value;
}
} // namespace boost::serialization
