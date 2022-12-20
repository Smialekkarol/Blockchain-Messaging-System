#pragma once

#include <algorithm>

#include "db/ConsensusStorage.hpp"
#include "db/RedisDB.hpp"

#include "SlotContext.hpp"

namespace slot {
class PendingBlockRemover {
public:
  PendingBlockRemover(SlotContext &context, ::db::RedisDB &redis,
                      ::db::ConsensusStorage &consensusStorage);

  void tryRemove();

private:
  SlotContext &context;
  ::db::RedisDB &redis;
  ::db::ConsensusStorage &consensusStorage;
};
} // namespace slot