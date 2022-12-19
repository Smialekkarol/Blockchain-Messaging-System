#include <algorithm>

#include "InspectionWaiter.hpp"

namespace slot {
InspectionWaiter::InspectionWaiter(SlotContext &context,
                                   ::db::ConsensusStorage &consensusStorage)
    : context{context}, consensusStorage{consensusStorage} {}

void InspectionWaiter::wait() {
  const auto slot = context.header.slot;
  const auto &nodes = context.nodeConfiguration.nodes;
  while (!areAllContributorsConfirmed(nodes, slot)) {
  }
}

bool InspectionWaiter::areAllContributorsConfirmed(
    const std::vector<common::NodeInfo> &nodes,
    const std::uint64_t slot) const {
  return std::all_of(
      nodes.begin(), nodes.end(), [this, slot](const auto &node) {
        return consensusStorage.isContributing(node.address, slot).has_value();
      });
}
} // namespace slot