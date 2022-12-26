#include <algorithm>
#include <chrono>
#include <mutex>

#include <spdlog/spdlog.h>

#include "common/utils/Time.hpp"

#include "ElectionNotifier.hpp"
#include "ElectionWaiter.hpp"

namespace slot {
ElectionWaiter::ElectionWaiter(SlotContext &context,
                               ::io::ChannelStore &channelStore,
                               ::db::ConsensusStorage &consensusStorage)
    : context{context}, channelStore{channelStore}, consensusStorage{
                                                        consensusStorage} {}

void ElectionWaiter::wait() {
  const auto slot = context.header.slot;
  const auto slotSynchronizationContext =
      consensusStorage.getSlotSynchronizationContext(slot);
  std::unique_lock<std::mutex> lock{slotSynchronizationContext->mutex,
                                    std::defer_lock};
  spdlog::debug("[{}] ElectionWaiter::wait waiting for nodes election values.",
               context.block.slot);
  while (!consensusStorage.areAllElectionValuesPresent(slot) ||
         !consensusStorage.isElectionValueUnique(
             context.nodeConfiguration.self.address, slot)) {
    lock.lock();
    // TODO: handle timeout
    slotSynchronizationContext->condition.wait(lock, [this, slot]() {
      return consensusStorage.areAllElectionValuesPresent(slot);
    });
    if (!consensusStorage.isElectionValueUnique(
            context.nodeConfiguration.self.address, slot)) {
      spdlog::error(
          "ElectionWaiter::ElectionWaiter, slot[{}] Duplicate "
          "election value. Removing duplicates and generating new values.",
          slot);
      consensusStorage.removeElectionValueDuplicates(slot);
      slotSynchronizationContext->isSynchronized.store(false);
      lock.unlock();
      ElectionNotifier electionNotifier{context, channelStore,
                                        consensusStorage};
      electionNotifier.notify();
    }
  }
  spdlog::debug(
      "[{}] ElectionWaiter::wait finished waiting for nodes election values.",
      context.block.slot);
}
} // namespace slot