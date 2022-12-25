#pragma once

#include "common/serialization/BlockSerializer.hpp"

#include "db/ConsensusStorage.hpp"
#include "io/HeaderSerializer.hpp"
#include "io/ChannelStore.hpp"

#include "SlotContext.hpp"

namespace slot {
class DataRelay {
public:
  DataRelay(SlotContext &context, ::io::ChannelStore &channelStore,
            ::db::ConsensusStorage &consensusStorage);
  void transfer();

private:
  std::string createMessage();

  SlotContext &context;
  ::io::ChannelStore &channelStore;
  ::db::ConsensusStorage &consensusStorage;
  ::serialization::BlockSerializer blockSerializer{};
  ::io::HeaderSerializer headerSerializer{};
};

} // namespace slot