#include "ElectionNotifier.hpp"
#include "node/src/io/ElectionValueWrapper.hpp"
#include "node/src/io/RandomNumberGenerator.hpp"
#include "node/src/io/Utils.hpp"

namespace slot {
ElectionNotifier::ElectionNotifier(SlotContext &context,
                                   ::io::ChannelStore &channelStore,
                                   ::db::ConsensusStorage &consensusStorage)
    : context{context}, channelStore{channelStore}, consensusStorage{
                                                        consensusStorage} {}

void ElectionNotifier::notify() {
  context.header.operation = ::io::ConsensusOperation::ELECTION;
  context.serializedHeader = headerSerializer.serialize(context.header);
  const auto &message = createMessage();
  const auto &consensusChannels =
      channelStore.getRemote(::io::ChannelType::CONSENSUS);

  for (auto *channel : consensusChannels) {
    channel->publish(message);
  }
}

std::string ElectionNotifier::createMessage() {
  static int counter{0};
  std::uint64_t electionValue{7};
  if (counter < 2) {
    counter++;
  } else {
    ::io::RandomNumberGenerator randomNumberGenerator{};
    electionValue = randomNumberGenerator.generate();
  }
  consensusStorage.fillElectionValue(context.nodeConfiguration.self.address,
                                     context.header.slot, electionValue);
  return ::io::merge(context.serializedHeader,
                     electionValueWrapperSerializer.serialize(
                         ::io::ElectionValueWrapper{electionValue}));
}
} // namespace slot