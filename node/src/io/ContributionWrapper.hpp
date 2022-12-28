#pragma once

namespace io {
struct ContributionWrapper {
  bool isContributing{false};
};
} // namespace io

namespace boost::serialization {
template <class Archive>
void serialize(Archive &archive,
               ::io::ContributionWrapper &electionValueWrapper,
               const unsigned int version) {
  archive &electionValueWrapper.isContributing;
}
} // namespace boost::serialization
