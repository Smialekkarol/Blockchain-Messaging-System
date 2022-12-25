#include <algorithm>

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
  PendingBlockSaver PendingBlockSaver{context, redis, buffer};
  PendingBlockSaver.save();
};

void SlotHandler::notifyNodesAboutContribution() {
  ContributionNotifier contributionNotifier{context, channelStore,
                                            consensusStorage};
  contributionNotifier.notify();
}

void SlotHandler::waitForNodesInspection() {
  InspectionWaiter inspectionWaiter{context, consensusStorage};
  inspectionWaiter.wait();
}

void SlotHandler::removePendingBlockIfNoOneIsContributing() {
  PendingBlockRemover pendingBlockRemover{context, redis, consensusStorage};
  pendingBlockRemover.tryRemove();
}

void SlotHandler::sendElectionValue() {
  ElectionNotifier electionNotifier{context, channelStore, consensusStorage};
  electionNotifier.notify();
}

void SlotHandler::waitForNodesElection() {
  ElectionWaiter electionWaiter{context, channelStore, consensusStorage};
  electionWaiter.wait();
}

void SlotHandler::nominateValidator() {
  consensusStorage.markValidator(context.header.slot);
}

void SlotHandler::transferDataToValidator() {
  DataRelay dataRelay{context, channelStore, consensusStorage};
  dataRelay.transfer();
}

void SlotHandler::waitForContributorsData() {
  // TODO: this needs to be tested in proper simulation
  ContributorsDataWaiter contributorsDataWaiter{context, consensusStorage};
  contributorsDataWaiter.wait();
}

void SlotHandler::broadcast() {
  // TODO: this needs to be tested in proper simulation
  BroadcastNotifier broadcastNotifier{context, consensusStorage, channelStore};
  broadcastNotifier.broadcast();
}

void SlotHandler::waitForBroadcast() {
  // TODO: this needs to be tested in proper simulation
  BroadcastWaiter broadcastWaiter{context, consensusStorage};
  broadcastWaiter.wait();
}

void SlotHandler::saveCompleteBlock() {
  CompleteBlockSaver completeBlockSaver{context, redis, consensusStorage};
  completeBlockSaver.save();
}

void SlotHandler::publishBlock() { consumer.publish(context.block); }
} // namespace slot
