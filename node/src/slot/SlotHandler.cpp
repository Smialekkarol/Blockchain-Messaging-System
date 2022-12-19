#include <algorithm>

#include "common/itf/Constants.hpp"

#include "node/src/io/ContributionWrapper.hpp"
#include "node/src/io/Utils.hpp"

#include "SlotHandler.hpp"

namespace slot {

SlotHandler::SlotHandler(const common::NodeConfiguration &nodeConfiguration_,
                         ::db::ConsensusStorage &consensusStorage_,
                         ::db::RedisDB &redis_, Buffer &buffer_,
                         Consumer &consumer_, ::io::ChannelStore &channelStore_)
    : nodeConfiguration{nodeConfiguration_},
      consensusStorage{consensusStorage_}, redis{redis_}, buffer{buffer_},
      consumer{consumer_}, channelStore{channelStore_} {}

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
    this->header = {::io::ConsensusOperation::INSPECTION,
                    nodeConfiguration.self.name, nodeConfiguration.self.address,
                    this->block.value().timestamp, this->block.value().slot};
    this->serializedHeader = headerSerializer.serialize(header);
    this->contributionWrapper = {true};
  } else {
    ::common::itf::Block emptyBlock{};
    this->contributionWrapper = {false};
    this->header = {::io::ConsensusOperation::INSPECTION,
                    nodeConfiguration.self.name, nodeConfiguration.self.address,
                    emptyBlock.timestamp, emptyBlock.slot};
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
  const auto slot = header.slot;
  std::unordered_map<std::string, std::optional<bool>> confirmedContributions{};
  confirmedContributions[nodeConfiguration.self.address] = block.has_value();
  for (const auto &node : nodeConfiguration.nodes) {
    confirmedContributions[node.address] = {};
  }

  const auto areAllValuesWithoutValues =
      std::all_of(confirmedContributions.begin(), confirmedContributions.end(),
                  [](const auto &pair) { return !pair.second.has_value(); });

  while (!areAllContributorsConfirmed(confirmedContributions)) {
    for (const auto &node : nodeConfiguration.nodes) {
      if (const auto &contribution =
              consensusStorage.isContributing(node.address, slot);
          contribution.has_value()) {
        confirmedContributions[node.address] = contribution.value();
      }
    }
  }

  const auto isAnyNodeContributing =
      std::any_of(confirmedContributions.begin(), confirmedContributions.end(),
                  [](const auto &pair) { return pair.second.value(); });

  if (!isAnyNodeContributing) {
    removePendingBlock();
    shouldCallNextHandler = false;
  }

  if (!block.has_value()) {
    shouldCallNextHandler = false;
  }
}

bool SlotHandler::areAllContributorsConfirmed(
    const std::unordered_map<std::string, std::optional<bool>>
        &confirmedContributors) const {
  return std::all_of(confirmedContributors.begin(), confirmedContributors.end(),
                     [](const auto &pair) { return pair.second.has_value(); });
}

void SlotHandler::removePendingBlock() {

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
