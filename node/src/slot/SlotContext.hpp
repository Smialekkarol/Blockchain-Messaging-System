#pragma once

#include <optional>
#include <string>

#include "common/NodeConfiguration.hpp"
#include "common/itf/Block.hpp"

#include "node/src/io/ContributionWrapper.hpp"
#include "node/src/io/Header.hpp"

namespace slot {
struct SlotContext {
  ::common::NodeConfiguration nodeConfiguration{};
  ::io::ContributionWrapper contributionWrapper{false};
  ::io::Header header{};
  std::string serializedHeader{};
  long long blockIndex{0};
  ::common::itf::Block block{};
  bool shouldCallNextHandler{true};
};
} // namespace slot