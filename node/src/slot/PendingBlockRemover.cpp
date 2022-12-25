#include <algorithm>

#include "common/itf/Constants.hpp"

#include "PendingBlockRemover.hpp"

namespace slot {
PendingBlockRemover::PendingBlockRemover(
    SlotContext &context, ::db::RedisDB &redis,
    ::db::ConsensusStorage &consensusStorage)
    : context{context}, redis{redis}, consensusStorage{consensusStorage} {}

void PendingBlockRemover::tryRemove() {
  const auto slot = context.header.slot;
  const auto &nodes = context.nodeConfiguration.nodes;
  if (!consensusStorage.isAnyNodeContributing(slot)) {
    const auto &b =
        redis.getByIndex(context.blockIndex, ::common::itf::DEFAULT_BLOCKAIN);
    redis.remove(b.value(), ::common::itf::DEFAULT_BLOCKAIN);
    context.shouldCallNextHandler = false;
  }
}
} // namespace slot