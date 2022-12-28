#include "common/itf/Constants.hpp"

#include "PendingBlockSaver.hpp"

#include <spdlog/spdlog.h>

namespace slot {
PendingBlockSaver::PendingBlockSaver(SlotContext &context, ::db::RedisDB &redis,
                                     Buffer &buffer,
                                     ::db::ConsensusStorage &consensusStorage)
    : context{context}, redis{redis}, buffer{buffer}, consensusStorage{
                                                          consensusStorage} {}

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
  consensusStorage.addBlock(context.nodeConfiguration.self.address,
                            context.header.slot, context.block);
  context.serializedHeader = headerSerializer.serialize(context.header);
  spdlog::debug("[{}]PendingBlockSaver::save. Header: {}", context.block.slot,
               context.serializedHeader);
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