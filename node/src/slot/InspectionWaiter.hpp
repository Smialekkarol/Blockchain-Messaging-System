#pragma once

#include "db/ConsensusStorage.hpp"

#include "SlotContext.hpp"

namespace slot {
class InspectionWaiter {
public:
  InspectionWaiter(SlotContext &context,
                   ::db::ConsensusStorage &consensusStorage);

  void wait();

private:
  bool areAllContributorsConfirmed(const std::vector<common::NodeInfo> &nodes,
                                   const std::uint64_t slot) const;

  SlotContext &context;
  ::db::ConsensusStorage &consensusStorage;
};

} // namespace slot