#include <algorithm>
#include <chrono>
#include <mutex>

#include <spdlog/spdlog.h>

#include "common/utils/Time.hpp"

#include "ContributorsDataWaiter.hpp"

namespace slot {
ContributorsDataWaiter::ContributorsDataWaiter(
    SlotContext &context, ::db::ConsensusStorage &consensusStorage)
    : context{context}, consensusStorage{consensusStorage} {}

void ContributorsDataWaiter::wait() {
  if (!context.isValidator) {
    spdlog::info("I am not validator so i don't wait for contribution data.");
    return;
  }
  spdlog::info("I am validator so i wait for contributors data.");
  const auto slot = context.header.slot;
  const auto slotSynchronizationContext =
      consensusStorage.getSlotSynchronizationContext(slot);
  if (consensusStorage.isAllContributorsDataCollected(slot)) {
    context.broadcastBlock = consensusStorage.mergeContributors(slot);
    consensusStorage.marAsResolved(context.nodeConfiguration.self.address,
                                   slot);
    slotSynchronizationContext->isSynchronized.store(false);
    spdlog::info(
        "I am validator and i have all contributors data - early return");
    return;
  }

  std::unique_lock<std::mutex> lock{slotSynchronizationContext->mutex};
  slotSynchronizationContext->condition.wait(lock, [this, slot]() {
    return consensusStorage.isAllContributorsDataCollected(slot);
  });
  context.broadcastBlock = consensusStorage.mergeContributors(slot);
  consensusStorage.marAsResolved(context.nodeConfiguration.self.address, slot);
  slotSynchronizationContext->isSynchronized.store(false);
  spdlog::info(
      "I am validator and i have all contributors data - actual return");
}
} // namespace slot