#pragma once

#include <algorithm>
#include <sstream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>

namespace serialization {
template <typename Data> class Serializer {
public:
  Serializer() = default;

  std::string serialize(const Data &data) {
    ss.str("");
    textOutputArchive << data;
    return ss.str();
  }

  std::vector<std::string> serialize(const std::vector<Data> &data) {
    std::vector<std::string> serializedBlocks{};
    std::transform(data.begin(), data.end(), serializedBlocks.begin(),
                   [&](const Data &b) { return serialize(b); });
    return serializedBlocks;
  }

  Data deserialize(const std::string &data) {
    ss.str("");
    Data deserializedBlock{};
    ss.str(data);
    inputTextArchive >> deserializedBlock;
    return deserializedBlock;
  }

  std::vector<Data> deserialize(const std::vector<std::string> &data) {
    std::vector<Data> deserializedBlocks{};
    std::transform(data.begin(), data.end(), deserializedBlocks.begin(),
                   [&](const std::string &b) { return deserialize(b); });
    return deserializedBlocks;
  }

private:
  std::stringstream ss{};
  boost::archive::text_iarchive inputTextArchive{ss};
  boost::archive::text_oarchive textOutputArchive{ss};
};
} // namespace serialization
