#include <algorithm>
#include <mutex>

#include <spdlog/spdlog.h>

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
  std::unique_lock<std::mutex> lock{slotSynchronizationContext->mutex};
  slotSynchronizationContext->condition.wait(
      lock, [slotSynchronizationContext] {
        return slotSynchronizationContext->isSynchronized.load();
      });

  if (!consensusStorage.isElectionValueUnique(
          context.nodeConfiguration.self.address, slot)) {
    spdlog::error("Duplicate election value");
    consensusStorage.removeElectionValueDuplicates(slot);
    slotSynchronizationContext->isSynchronized.store(false);
    ElectionNotifier electionNotifier{context, channelStore, consensusStorage};
    electionNotifier.notify();
    wait();
  }
}
} // namespace slot