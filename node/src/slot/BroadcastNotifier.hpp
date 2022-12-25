#pragma once

#include "common/serialization/BlockSerializer.hpp"

#include "db/ConsensusStorage.hpp"

#include "io/ChannelStore.hpp"
#include "io/ElectionValueWrapperSerializer.hpp"
#include "io/HeaderSerializer.hpp"

#include "SlotContext.hpp"

namespace slot {
class BroadcastNotifier {
public:
  BroadcastNotifier(SlotContext &context,
                    ::db::ConsensusStorage &consensusStorage,
                    ::io::ChannelStore &channelStore);
  void broadcast();

private:
  std::string createMessage();

  SlotContext &context;
  ::io::ChannelStore &channelStore;
  ::db::ConsensusStorage &consensusStorage;
  ::io::HeaderSerializer headerSerializer{};
  ::serialization::BlockSerializer blockSerializer{};
};

} // namespace slot