#pragma once

#include "db/ConsensusStorage.hpp"
#include "node/src/io/ChannelStore.hpp"
#include "node/src/io/ContributionWrapperSerializer.hpp"

#include "SlotContext.hpp"

namespace slot {
class ContributionNotifier {
public:
  ContributionNotifier(SlotContext &context, ::io::ChannelStore &channelStore,
                       ::db::ConsensusStorage &consensusStorage);
  void notify();

private:
  std::string createMessage();

  SlotContext &context;
  ::io::ChannelStore &channelStore;
  ::db::ConsensusStorage &consensusStorage;
  ::io::ContributionWrapperSerializer contributionWrapperSerializer{};
};

} // namespace slot