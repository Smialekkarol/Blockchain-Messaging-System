#include <algorithm>

#include "common/itf/Constants.hpp"

#include "PendingBlockRemover.hpp"

#include <spdlog/spdlog.h>

namespace slot {
PendingBlockRemover::PendingBlockRemover(
    SlotContext &context, ::db::RedisDB &redis,
    ::db::ConsensusStorage &consensusStorage)
    : context{context}, redis{redis}, consensusStorage{consensusStorage} {}

void PendingBlockRemover::tryRemove() {
  const auto slot = context.header.slot;
  const auto &nodes = context.nodeConfiguration.nodes;
  spdlog::debug("[{}] PendingBlockRemover::tryRemove", context.block.slot);
  if (!consensusStorage.isAnyNodeContributing(slot)) {
    spdlog::debug("[{}] PendingBlockRemover::tryRemove there is not "
                 "contribution, deleting placeholder pending block and closing "
                 "SlotHandler. ",
                 context.block.slot);
    const auto &b =
        redis.getByIndex(context.blockIndex, ::common::itf::DEFAULT_BLOCKAIN);
    redis.remove(b.value(), ::common::itf::DEFAULT_BLOCKAIN);
    context.shouldCallNextHandler = false;
  }
}
} // namespace slot