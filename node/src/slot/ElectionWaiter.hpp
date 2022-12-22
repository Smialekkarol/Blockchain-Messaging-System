#pragma once

#include "db/ConsensusStorage.hpp"
#include "io/ChannelStore.hpp"

#include "SlotContext.hpp"

namespace slot {
class ElectionWaiter {
public:
  ElectionWaiter(SlotContext &context, ::io::ChannelStore &channelStore,
                 ::db::ConsensusStorage &consensusStorage);

  void wait();

private:
  SlotContext &context;
  ::io::ChannelStore &channelStore;
  ::db::ConsensusStorage &consensusStorage;
};

} // namespace slot