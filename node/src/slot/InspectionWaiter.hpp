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
  SlotContext &context;
  ::db::ConsensusStorage &consensusStorage;
};

} // namespace slot