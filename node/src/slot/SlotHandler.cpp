#include <algorithm>

#include "common/itf/Constants.hpp"

#include "ContributionNotifier.hpp"
#include "InspectionWaiter.hpp"
#include "PendingBlockSaver.hpp"

#include "SlotHandler.hpp"

namespace slot {
SlotHandler::SlotHandler(const common::NodeConfiguration &nodeConfiguration_,
                         ::db::ConsensusStorage &consensusStorage_,
                         ::db::RedisDB &redis_, Buffer &buffer_,
                         Consumer &consumer_, ::io::ChannelStore &channelStore_)
    : consensusStorage{consensusStorage_}, redis{redis_}, buffer{buffer_},
      consumer{consumer_}, channelStore{channelStore_} {
  context.nodeConfiguration = nodeConfiguration_;
}

void SlotHandler::handle() {
  context.shouldCallNextHandler = true;
  for (const auto handler : handlers) {
    if (!context.shouldCallNextHandler) {
      break;
    }
    handler();
  }
}

void SlotHandler::savePendingBlock() {
  PendingBlockSaver PendingBlockSaver{context, redis, buffer};
  PendingBlockSaver.save();
};

void SlotHandler::notifyNodesAboutContribution() {
  ContributionNotifier contributionNotifier{context, channelStore};
  contributionNotifier.notify();
}

void SlotHandler::waitForNodesInspection() {
  InspectionWaiter inspectionWaiter{context, consensusStorage};
  inspectionWaiter.wait();
}

void SlotHandler::removePendingBlockIfNoOneIsContributing() {
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

void SlotHandler::saveCompleteBlock() {
  context.block.state = ::common::itf::BlockState::COMPLETED;
  redis.update(context.block, context.blockIndex,
               ::common::itf::DEFAULT_BLOCKAIN);
  consensusStorage.clearSlot(context.header.slot);
}

void SlotHandler::publishBlock() { consumer.publish(context.block); }
} // namespace slot
