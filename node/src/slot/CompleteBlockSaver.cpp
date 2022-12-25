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
  // context.broadcastBlock.state = ::common::itf::BlockState::COMPLETED;
  // spdlog::info("CompleteBlockSaver::save start");
  // redis.update(context.broadcastBlock, context.blockIndex,
  //              ::common::itf::DEFAULT_BLOCKAIN);
  // spdlog::info("CompleteBlockSaver::save after redis update");
  // consensusStorage.clearSlot(context.header.slot);
  // spdlog::info("CompleteBlockSaver::save after clearslot");

  context.block.state = ::common::itf::BlockState::COMPLETED;
  spdlog::info("CompleteBlockSaver::save start");
  redis.update(context.block, context.blockIndex,
               ::common::itf::DEFAULT_BLOCKAIN);
  spdlog::info("CompleteBlockSaver::save after redis update");
  consensusStorage.clearSlot(context.header.slot);
  spdlog::info("CompleteBlockSaver::save after clearslot");
}
} // namespace slot