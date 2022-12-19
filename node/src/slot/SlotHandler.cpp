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
  buffer.save();
  if (const auto b = buffer.pop(); b.has_value()) {
    context.block = b.value();
    const auto blockchainSize =
        redis.add(context.block.value(), ::common::itf::DEFAULT_BLOCKAIN);
    context.blockIndex = blockchainSize - 1;
    context.header = {::io::ConsensusOperation::INSPECTION,
                      context.nodeConfiguration.self.name,
                      context.nodeConfiguration.self.address,
                      context.block.value().timestamp,
                      context.block.value().slot};
    context.serializedHeader = headerSerializer.serialize(context.header);
    context.contributionWrapper = {true};
  } else {
    ::common::itf::Block emptyBlock{};
    context.contributionWrapper = {false};
    context.header = {::io::ConsensusOperation::INSPECTION,
                      context.nodeConfiguration.self.name,
                      context.nodeConfiguration.self.address,
                      emptyBlock.timestamp, emptyBlock.slot};
    context.serializedHeader = headerSerializer.serialize(context.header);
    const auto blockchainSize =
        redis.add(emptyBlock, ::common::itf::DEFAULT_BLOCKAIN);
    context.blockIndex = blockchainSize - 1;
  }

  const auto &message = ::io::merge(
      context.serializedHeader,
      contributionWrapperSerializer.serialize(context.contributionWrapper));

  const auto &consensusChannels =
      channelStore.getRemote(::io::ChannelType::CONSENSUS);

  for (auto *channel : consensusChannels) {
    channel->publish(message);
  }
};

void SlotHandler::notifyNodesAboutPendingBlock() {}

void SlotHandler::waitForNodesInspection() {
  const auto slot = context.header.slot;
  std::unordered_map<std::string, std::optional<bool>> confirmedContributions{};
  confirmedContributions[context.nodeConfiguration.self.address] =
      context.block.has_value();
  for (const auto &node : context.nodeConfiguration.nodes) {
    confirmedContributions[node.address] = {};
  }

  const auto areAllValuesWithoutValues =
      std::all_of(confirmedContributions.begin(), confirmedContributions.end(),
                  [](const auto &pair) { return !pair.second.has_value(); });

  while (!areAllContributorsConfirmed(confirmedContributions)) {
    for (const auto &node : context.nodeConfiguration.nodes) {
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
    context.shouldCallNextHandler = false;
  }

  if (!context.block.has_value()) {
    context.shouldCallNextHandler = false;
  }
}

bool SlotHandler::areAllContributorsConfirmed(
    const std::unordered_map<std::string, std::optional<bool>>
        &confirmedContributors) const {
  return std::all_of(confirmedContributors.begin(), confirmedContributors.end(),
                     [](const auto &pair) { return pair.second.has_value(); });
}

void SlotHandler::removePendingBlock() {

  const auto &b =
      redis.getByIndex(context.blockIndex, ::common::itf::DEFAULT_BLOCKAIN);
  redis.remove(b.value(), ::common::itf::DEFAULT_BLOCKAIN);
}

void SlotHandler::saveCompleteBlock() {
  context.block.value().state = ::common::itf::BlockState::COMPLETED;
  redis.update(context.block.value(), context.blockIndex,
               ::common::itf::DEFAULT_BLOCKAIN);
  consensusStorage.clearSlot(context.header.slot);
}

void SlotHandler::publishBlock() { consumer.publish(context.block.value()); }
} // namespace slot
