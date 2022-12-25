#pragma once

#include "db/ConsensusStorage.hpp"

#include "SlotContext.hpp"

namespace slot {
class NominatorValidator {
public:
  NominatorValidator(SlotContext &context,
                     ::db::ConsensusStorage &consensusStorage);

  void nominate();

private:
  SlotContext &context;
  ::db::ConsensusStorage &consensusStorage;
};

} // namespace slot