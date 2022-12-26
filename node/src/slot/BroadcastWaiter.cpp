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
    spdlog::info("I am validator so i don't wait for bradcast");
    return;
  }
  spdlog::info("I am not validator and i am starting waiting");
  const auto slot = context.header.slot;
  const auto slotSynchronizationContext =
      consensusStorage.getSlotSynchronizationContext(slot);
  const auto result = consensusStorage.isResolved(context.validator, slot);
  spdlog::info("result {} ", result);
  if (result) {
    context.broadcastBlock = consensusStorage.getBlock(context.validator, slot);
    slotSynchronizationContext->isSynchronized.store(false);
    spdlog::info("Early return I am not validator and i have finished waiting "
                 "for validator "
                 "resolution: {} from {} at slot {}",
                 consensusStorage.isResolved(context.validator, slot),
                 context.validator, slot);
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
  spdlog::info("The actual waiting I am not validator and i have finished "
               "waiting for validator "
               "resolution: {} from {} at slot {}",
               consensusStorage.isResolved(context.validator, slot),
               context.validator, slot);
}
} // namespace slot