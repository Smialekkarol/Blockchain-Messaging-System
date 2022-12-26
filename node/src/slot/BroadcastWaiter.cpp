#include <algorithm>
#include <chrono>
#include <mutex>

#include <spdlog/spdlog.h>

#include "common/utils/Time.hpp"

#include "BroadcastWaiter.hpp"

namespace slot {
BroadcastWaiter::BroadcastWaiter(SlotContext &context,
                                 ::db::ConsensusStorage &consensusStorage)
    : context{context}, consensusStorage{consensusStorage} {}

void BroadcastWaiter::wait() {
  if (context.isValidator) {
    spdlog::debug("[{}] BroadcastWaiter::wait I am validator so i don't wait "
                 "for bradcast.",
                 context.header.slot);
    return;
  }
  spdlog::debug(
      "[{}] BroadcastWaiter::wait I am not validator and i am starting waiting",
      context.header.slot);
  const auto slot = context.header.slot;
  const auto slotSynchronizationContext =
      consensusStorage.getSlotSynchronizationContext(slot);
  if (consensusStorage.isResolved(context.validator, slot)) {
    context.broadcastBlock = consensusStorage.getBlock(context.validator, slot);
    slotSynchronizationContext->isSynchronized.store(false);
    spdlog::debug("[{}] BroadcastWaiter::wait I am not validator and i have "
                 "finished waiting early return.",
                 context.header.slot);
    return;
  }

  std::unique_lock<std::mutex> lock{slotSynchronizationContext->mutex};
  slotSynchronizationContext->condition.wait(
      lock, [&slotSynchronizationContext, this, slot]() {
        return slotSynchronizationContext->isSynchronized.load() &&
               consensusStorage.isResolved(context.validator, slot);
      });
  context.broadcastBlock = consensusStorage.getBlock(context.validator, slot);
  slotSynchronizationContext->isSynchronized.store(false);

  spdlog::debug("[{}] BroadcastWaiter::wait I am not validator and i have "
               "finished waiting actual return.",
               context.header.slot);
}
} // namespace slot