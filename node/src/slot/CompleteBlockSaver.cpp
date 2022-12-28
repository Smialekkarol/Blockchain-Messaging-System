#include <algorithm>

#include "common/itf/Constants.hpp"

#include "CompleteBlockSaver.hpp"

#include <spdlog/spdlog.h>

namespace slot {
CompleteBlockSaver::CompleteBlockSaver(SlotContext &context,
                                       ::db::RedisDB &redis,
                                       ::db::ConsensusStorage &consensusStorage)
    : context{context}, redis{redis}, consensusStorage{consensusStorage} {}

void CompleteBlockSaver::save() {
  spdlog::debug("[{}] CompleteBlockSaver::save saving broadcasted block and "
               "clearing slot consensus context",
               context.header.slot);
  context.block.state = ::common::itf::BlockState::COMPLETED;
  redis.update(context.broadcastBlock, context.blockIndex,
               ::common::itf::DEFAULT_BLOCKAIN);
  consensusStorage.clearSlot(context.header.slot);
}
} // namespace slot