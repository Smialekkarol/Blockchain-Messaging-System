#include "node/src/io/Utils.hpp"

#include "ContributionNotifier.hpp"

#include <spdlog/spdlog.h>

namespace slot {
ContributionNotifier::ContributionNotifier(
    SlotContext &context, ::io::ChannelStore &channelStore,
    ::db::ConsensusStorage &consensusStorage)
    : context{context}, channelStore{channelStore}, consensusStorage{
                                                        consensusStorage} {}

void ContributionNotifier::notify() {
  consensusStorage.addContext(context.header.address, context.header.node,
                              context.header.slot);
  consensusStorage.setContribution(context.nodeConfiguration.self.address,
                                   context.header.slot,
                                   context.contributionWrapper.isContributing);
  const auto &message = createMessage();
  const auto &consensusChannels =
      channelStore.getRemote(::io::ChannelType::CONSENSUS);

  spdlog::debug("[{}] ContributionNotifier::notify. Sending to channels: {}",
               context.header.slot, message);

  for (auto *channel : consensusChannels) {
    channel->publish(message);
  }
}

std::string ContributionNotifier::createMessage() {
  return ::io::merge(
      context.serializedHeader,
      contributionWrapperSerializer.serialize(context.contributionWrapper));
}
} // namespace slot