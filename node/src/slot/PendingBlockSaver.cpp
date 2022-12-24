#include "common/itf/Constants.hpp"

#include "PendingBlockSaver.hpp"

namespace slot {
PendingBlockSaver::PendingBlockSaver(SlotContext &context, ::db::RedisDB &redis,
                                     Buffer &buffer)
    : context{context}, redis{redis}, buffer{buffer} {}

void PendingBlockSaver::save() {
  buffer.save();
  if (const auto b = buffer.pop(); b.has_value()) {
    context.block = b.value();
    context.contributionWrapper = {true};
  } else {
    context.block = {};
    context.contributionWrapper = {false};
  }
  saveBlockToDb();
  saveHeader();
  context.serializedHeader = headerSerializer.serialize(context.header);
}

void PendingBlockSaver::saveBlockToDb() {
  const auto blockchainSize =
      redis.add(context.block, ::common::itf::DEFAULT_BLOCKAIN);
  context.blockIndex = blockchainSize - 1;
}

void PendingBlockSaver::saveHeader() {
  context.header = {::io::ConsensusOperation::INSPECTION,
                    context.nodeConfiguration.self.name,
                    context.nodeConfiguration.self.address,
                    context.block.timestamp, context.block.slot};
}
} // namespace slot