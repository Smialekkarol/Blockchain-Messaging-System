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
    spdlog::debug("[{}] SlotHandler::broadcast I am not validator so i don't do "
                 "broadcast. Early return.",
                 context.header.slot);
    return;
  }

  const auto &message = createMessage();
  const auto &consensusChannels =
      channelStore.getRemote(::io::ChannelType::CONSENSUS);
  spdlog::debug("[{}] SlotHandler::broadcast I am not validator, so i broadcast "
               "message: {}",
               context.header.slot, message);
  for (auto *channel : consensusChannels) {
    channel->publish(message);
  }
}

std::string BroadcastNotifier::createMessage() {
  return ::io::merge(context.serializedHeader,
                     blockSerializer.serialize(context.broadcastBlock));
}
} // namespace slot