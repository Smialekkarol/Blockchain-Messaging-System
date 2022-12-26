#include <algorithm>

#include <spdlog/spdlog.h>

#include "common/itf/Constants.hpp"

#include "BroadcastNotifier.hpp"
#include "BroadcastWaiter.hpp"
#include "CompleteBlockSaver.hpp"
#include "ContributionNotifier.hpp"
#include "ContributorsDataWaiter.hpp"
#include "DataRelay.hpp"
#include "ElectionNotifier.hpp"
#include "ElectionWaiter.hpp"
#include "InspectionWaiter.hpp"
#include "PendingBlockRemover.hpp"
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
  spdlog::debug("[{}] SlotHandler::savePendingBlock", context.block.slot);
  PendingBlockSaver PendingBlockSaver{context, redis, buffer, consensusStorage};
  PendingBlockSaver.save();
};

void SlotHandler::notifyNodesAboutContribution() {
  spdlog::debug("[{}] SlotHandler::notifyNodesAboutContribution",
                context.header.slot);
  ContributionNotifier contributionNotifier{context, channelStore,
                                            consensusStorage};
  contributionNotifier.notify();
}

void SlotHandler::waitForNodesInspection() {
  spdlog::debug("[{}] SlotHandler::waitForNodesInspection",
                context.header.slot);
  InspectionWaiter inspectionWaiter{context, consensusStorage};
  inspectionWaiter.wait();
}

void SlotHandler::removePendingBlockIfNoOneIsContributing() {
  spdlog::debug("[{}] SlotHandler::removePendingBlockIfNoOneIsContributing",
                context.header.slot);
  PendingBlockRemover pendingBlockRemover{context, redis, consensusStorage};
  pendingBlockRemover.tryRemove();
}

void SlotHandler::sendElectionValue() {
  spdlog::debug("[{}] SlotHandler::sendElectionValue", context.header.slot);
  ElectionNotifier electionNotifier{context, channelStore, consensusStorage};
  electionNotifier.notify();
}

void SlotHandler::waitForNodesElection() {
  spdlog::debug("[{}] SlotHandler::waitForNodesElection", context.header.slot);
  ElectionWaiter electionWaiter{context, channelStore, consensusStorage};
  electionWaiter.wait();
}

void SlotHandler::nominateValidator() {
  spdlog::debug("[{}] SlotHandler::nominateValidator", context.header.slot);
  consensusStorage.markValidator(context.header.slot);
}

void SlotHandler::transferDataToValidator() {
  spdlog::debug("[{}] SlotHandler::transferDataToValidator",
                context.header.slot);
  DataRelay dataRelay{context, channelStore, consensusStorage};
  dataRelay.transfer();
}

void SlotHandler::waitForContributorsData() {
  spdlog::debug("[{}] SlotHandler::waitForContributorsData",
                context.header.slot);
  // TODO: this needs to be tested in proper simulation
  ContributorsDataWaiter contributorsDataWaiter{context, consensusStorage};
  contributorsDataWaiter.wait();
}

void SlotHandler::broadcast() {
  spdlog::debug("[{}] SlotHandler::broadcast", context.header.slot);
  // TODO: this needs to be tested in proper simulation
  BroadcastNotifier broadcastNotifier{context, consensusStorage, channelStore};
  broadcastNotifier.broadcast();
}

void SlotHandler::waitForBroadcast() {
  spdlog::debug("[{}] SlotHandler::waitForBroadcast", context.header.slot);
  // TODO: this needs to be tested in proper simulation
  BroadcastWaiter broadcastWaiter{context, consensusStorage};
  broadcastWaiter.wait();
}

void SlotHandler::saveCompleteBlock() {
  spdlog::debug("[{}] SlotHandler::saveCompleteBlock", context.header.slot);
  CompleteBlockSaver completeBlockSaver{context, redis, consensusStorage};
  completeBlockSaver.save();
}

void SlotHandler::publishBlock() {
  spdlog::debug("[{}] SlotHandler::publishBlock", context.header.slot);
  consumer.publish(context.broadcastBlock, context.header.slot);
}
} // namespace slot
