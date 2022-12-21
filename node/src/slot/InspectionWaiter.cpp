#include <algorithm>
#include <mutex>

#include "InspectionWaiter.hpp"

namespace slot {
InspectionWaiter::InspectionWaiter(SlotContext &context,
                                   ::db::ConsensusStorage &consensusStorage)
    : context{context}, consensusStorage{consensusStorage} {}

void InspectionWaiter::wait() {
  const auto slot = context.header.slot;
  const auto slotSynchronizationContext =
      consensusStorage.getSlotSynchronizationContext(slot);
  std::unique_lock<std::mutex> lock{slotSynchronizationContext->mutex};
  slotSynchronizationContext->condition.wait(
      lock, [slotSynchronizationContext] {
        return slotSynchronizationContext->isSynchronized.load();
      });
  slotSynchronizationContext->isSynchronized.store(false);
}
} // namespace slot