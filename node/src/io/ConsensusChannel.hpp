#pragma once

#include "Channel.hpp"

namespace io {
class ConsensusChannel : public Channel {
public:
  ConsensusChannel(const ::io::ConnectionData &connectionData)
      : Channel(connectionData, ChannelType::CONSENSUS) {}
};
} // namespace io