#include "BroadcastNotifier.hpp"
#include "node/src/io/ElectionValueWrapper.hpp"
#include "node/src/io/RandomNumberGenerator.hpp"
#include "node/src/io/Utils.hpp"

#include <spdlog/spdlog.h>

namespace slot {
BroadcastNotifier::BroadcastNotifier(SlotContext &context,
                                     ::db::ConsensusStorage &consensusStorage,
                                     ::io::ChannelStore &channelStore)
    : context{context}, channelStore{channelStore}, consensusStorage{
                                                        consensusStorage} {}

void BroadcastNotifier::broadcast() {
  context.header.operation = ::io::ConsensusOperation::BROADCAST;
  context.serializedHeader = headerSerializer.serialize(context.header);

  if (!context.isValidator) {
    spdlog::info("I am not validator so i don't do bradcast, i will start "
                 "waiting for broadcast");
    return;
  }

  const auto &message = createMessage();
  const auto &consensusChannels =
      channelStore.getRemote(::io::ChannelType::CONSENSUS);
  spdlog::info("I am validator so i start sendind broadcast");
  for (auto *channel : consensusChannels) {
    channel->publish(message);
  }

  spdlog::info("I am validator and i have finished broadcast");
}

std::string BroadcastNotifier::createMessage() {
  return ::io::merge(context.serializedHeader,
                     blockSerializer.serialize(context.block));
}
} // namespace slot