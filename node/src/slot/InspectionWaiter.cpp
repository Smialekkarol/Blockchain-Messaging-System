#include <algorithm>
#include <chrono>
#include <mutex>

#include <spdlog/spdlog.h>

#include "common/utils/Time.hpp"

#include "InspectionWaiter.hpp"

namespace slot {
InspectionWaiter::InspectionWaiter(SlotContext &context,
                                   ::db::ConsensusStorage &consensusStorage)
    : context{context}, consensusStorage{consensusStorage} {}

void InspectionWaiter::wait() {
  const auto slot = context.header.slot;
  const auto slotSynchronizationContext =
      consensusStorage.getSlotSynchronizationContext(slot);
  if (consensusStorage.areAllContributorsConfirmed(slot)) {
    slotSynchronizationContext->isSynchronized.store(false);
    return;
  }

  std::unique_lock<std::mutex> lock{slotSynchronizationContext->mutex};
  slotSynchronizationContext->condition.wait(
      lock, [slotSynchronizationContext]() {
        return slotSynchronizationContext->isSynchronized.load();
      });
  slotSynchronizationContext->isSynchronized.store(false);
}
} // namespace slot