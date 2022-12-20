#include <algorithm>

#include "common/itf/Constants.hpp"

#include "CompleteBlockSaver.hpp"

namespace slot {
CompleteBlockSaver::CompleteBlockSaver(SlotContext &context,
                                       ::db::RedisDB &redis,
                                       ::db::ConsensusStorage &consensusStorage)
    : context{context}, redis{redis}, consensusStorage{consensusStorage} {}

void CompleteBlockSaver::save() {
  context.block.state = ::common::itf::BlockState::COMPLETED;
  redis.update(context.block, context.blockIndex,
               ::common::itf::DEFAULT_BLOCKAIN);
  consensusStorage.clearSlot(context.header.slot);
}
} // namespace slot