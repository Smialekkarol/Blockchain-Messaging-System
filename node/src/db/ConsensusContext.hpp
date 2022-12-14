#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "common/itf/Block.hpp"

namespace db {
struct ConsensusContext {
  std::string node{};
  std::string address{};
  std::uint64_t slot{0};
  std::optional<bool> isContributing{};
  bool isValidator{false};
  std::uint64_t electionValue{0};
  ::common::itf::Block block{};
  bool isResolved{false};
};
} // namespace db
