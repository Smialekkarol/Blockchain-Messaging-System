#include <algorithm>
#include <chrono>
#include <mutex>

#include <spdlog/spdlog.h>

#include "common/utils/Time.hpp"

#include "ElectionNotifier.hpp"
#include "NominatorValidator.hpp"

namespace slot {
NominatorValidator::NominatorValidator(SlotContext &context,
                                       ::db::ConsensusStorage &consensusStorage)
    : context{context}, consensusStorage{consensusStorage} {}

void NominatorValidator::nominate() {
  const auto slot = context.header.slot;
}
} // namespace slot