#include "common/itf/Constants.hpp"

#include "node/src/io/ContributionWrapper.hpp"
#include "node/src/io/Utils.hpp"

#include "SlotHandler.hpp"

namespace slot {

SlotHandler::SlotHandler(const ::common::NodeInfo &nodeInfo_,
                         ::db::ConsensusStorage &consensusStorage_,
                         ::db::RedisDB &redis_, Buffer &buffer_,
                         Consumer &consumer_, ::io::ChannelStore &channelStore_)
    : nodeInfo{nodeInfo_}, consensusStorage{consensusStorage_}, redis{redis_},
      buffer{buffer_}, consumer{consumer_}, channelStore{channelStore_} {}

void SlotHandler::handle() {
  shouldCallNextHandler = true;
  for (const auto handler : handlers) {
    if (!shouldCallNextHandler) {
      break;
    }
    handler();
  }
}

void SlotHandler::savePendingBlock() {
  buffer.save();
  if (const auto b = buffer.pop(); b.has_value()) {
    this->block = b.value();
    const auto blockchainSize =
        redis.add(this->block.value(), ::common::itf::DEFAULT_BLOCKAIN);
    blockIndex = blockchainSize - 1;
    this->header = {::io::ConsensusOperation::INSPECTION, nodeInfo.name,
                    nodeInfo.address, this->block.value().timestamp,
                    this->block.value().slot};
    this->serializedHeader = headerSerializer.serialize(header);
    this->contributionWrapper = {true};
  } else {
    ::common::itf::Block emptyBlock{};
    this->contributionWrapper = {false};
    this->header = {::io::ConsensusOperation::INSPECTION, nodeInfo.name,
                    nodeInfo.address, emptyBlock.timestamp, emptyBlock.slot};
    this->serializedHeader = headerSerializer.serialize(header);
    const auto blockchainSize =
        redis.add(emptyBlock, ::common::itf::DEFAULT_BLOCKAIN);
    blockIndex = blockchainSize - 1;
  }

  const auto &message =
      ::io::merge(serializedHeader,
                  contributionWrapperSerializer.serialize(contributionWrapper));

  const auto &consensusChannels =
      channelStore.getRemote(::io::ChannelType::CONSENSUS);

  for (auto *channel : consensusChannels) {
    channel->publish(message);
  }
};

void SlotHandler::notifyNodesAboutPendingBlock() {}

void SlotHandler::waitForNodesInspection() {
  // TODO: Provide waiting implementation
}

void SlotHandler::removePendingBlock() {
  if (block.has_value()) {
    return;
  }
  // TODO: this setting shouldCallNextHandler is temporary, as in future we will
  // need to get information from other nodes.
  shouldCallNextHandler = false;
  const auto &b = redis.getByIndex(blockIndex, ::common::itf::DEFAULT_BLOCKAIN);
  redis.remove(b.value(), ::common::itf::DEFAULT_BLOCKAIN);
}

void SlotHandler::saveCompleteBlock() {
  block.value().state = ::common::itf::BlockState::COMPLETED;
  redis.update(block.value(), blockIndex, ::common::itf::DEFAULT_BLOCKAIN);
  consensusStorage.clearSlot(header.slot);
}

void SlotHandler::publishBlock() { consumer.publish(block.value()); }
} // namespace slot
