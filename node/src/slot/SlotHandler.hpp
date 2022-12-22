#pragma once

#include <functional>
#include <vector>

#include "SlotContext.hpp"
#include "db/ConsensusStorage.hpp"
#include "db/RedisDB.hpp"
#include "node/src/Buffer.hpp"
#include "node/src/Consumer.hpp"
#include "node/src/io/ChannelStore.hpp"

namespace slot {
class SlotHandler {
public:
  SlotHandler(const ::common::NodeConfiguration &nodeConfiguration_,
              ::db::ConsensusStorage &consensusStorage_, ::db::RedisDB &redis_,
              Buffer &buffer_, Consumer &consumer,
              ::io::ChannelStore &channelStore_);

  void handle();

private:
  void savePendingBlock();
  void notifyNodesAboutContribution();
  void waitForNodesInspection();
  void removePendingBlockIfNoOneIsContributing();
  void sendElectionValue();
  void waitForNodesElection();
  void saveCompleteBlock();
  void publishBlock();

  ::db::ConsensusStorage &consensusStorage;
  ::db::RedisDB &redis;
  Buffer &buffer;
  Consumer &consumer;
  ::io::ChannelStore &channelStore;
  ::serialization::BlockSerializer blockSerializer{};
  SlotContext context{};

  std::vector<std::function<void()>> handlers{
      [this]() { savePendingBlock(); },
      [this]() { notifyNodesAboutContribution(); },
      [this]() { waitForNodesInspection(); },
      [this]() { removePendingBlockIfNoOneIsContributing(); },
      [this]() { sendElectionValue(); },
      [this]() { waitForNodesElection(); },
      [this]() { saveCompleteBlock(); },
      [this]() { publishBlock(); }};
};
} // namespace slot