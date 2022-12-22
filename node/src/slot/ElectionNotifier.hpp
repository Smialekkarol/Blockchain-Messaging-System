#pragma once

#include "db/ConsensusStorage.hpp"

#include "io/ChannelStore.hpp"
#include "io/ElectionValueWrapperSerializer.hpp"
#include "io/HeaderSerializer.hpp"

#include "SlotContext.hpp"

namespace slot {
class ElectionNotifier {
public:
  ElectionNotifier(SlotContext &context, ::io::ChannelStore &channelStore,
                   ::db::ConsensusStorage &consensusStorage);
  void notify();

private:
  std::string createMessage();

  SlotContext &context;
  ::io::ChannelStore &channelStore;
  ::db::ConsensusStorage &consensusStorage;
  ::io::ElectionValueWrapperSerializer electionValueWrapperSerializer{};
  ::io::HeaderSerializer headerSerializer{};
};

} // namespace slot