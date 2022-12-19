#pragma once

#include <functional>
// #include <optional>
// #include <string>
#include <vector>

// #include "common/NodeConfiguration.hpp"

#include "SlotContext.hpp"
#include "db/ConsensusStorage.hpp"
#include "db/RedisDB.hpp"
#include "node/src/Buffer.hpp"
#include "node/src/Consumer.hpp"
#include "node/src/io/ChannelStore.hpp"
#include "node/src/io/ContributionWrapperSerializer.hpp"
#include "node/src/io/HeaderSerializer.hpp"

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
  void notifyNodesAboutPendingBlock();
  void waitForNodesInspection();
  void removePendingBlock();
  void saveCompleteBlock();
  void publishBlock();

  bool areAllContributorsConfirmed(
      const std::unordered_map<std::string, std::optional<bool>>
          &confirmedContributors) const;

  ::db::ConsensusStorage &consensusStorage;
  ::db::RedisDB &redis;
  Buffer &buffer;
  Consumer &consumer;
  ::io::ChannelStore &channelStore;
  ::serialization::BlockSerializer blockSerializer{};
  ::io::HeaderSerializer headerSerializer{};
  ::io::ContributionWrapperSerializer contributionWrapperSerializer{};
  SlotContext context{};

  std::vector<std::function<void()>> handlers{
      [this]() { savePendingBlock(); },
      [this]() { notifyNodesAboutPendingBlock(); },
      [this]() { waitForNodesInspection(); }, [this]() { saveCompleteBlock(); },
      [this]() { publishBlock(); }};
};
} // namespace slot