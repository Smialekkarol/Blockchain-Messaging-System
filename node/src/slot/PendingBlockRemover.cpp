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
  const auto isAnyNodeContributing =
      std::any_of(
          nodes.begin(), nodes.end(),
          [this, slot](const auto &node) {
            return consensusStorage.isContributing(node.address, slot).value();
          }) ||
      context.contributionWrapper.isContributing;

  if (!isAnyNodeContributing) {
    const auto &b =
        redis.getByIndex(context.blockIndex, ::common::itf::DEFAULT_BLOCKAIN);
    redis.remove(b.value(), ::common::itf::DEFAULT_BLOCKAIN);
    context.shouldCallNextHandler = false;
  }
}
} // namespace slot