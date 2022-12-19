#include "node/src/io/Utils.hpp"

#include "ContributionNotifier.hpp"

namespace slot {
ContributionNotifier::ContributionNotifier(SlotContext &context,
                                           ::io::ChannelStore &channelStore)
    : context{context}, channelStore{channelStore} {}

void ContributionNotifier::notify() {
  const auto &message = createMessage();
  const auto &consensusChannels =
      channelStore.getRemote(::io::ChannelType::CONSENSUS);

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