#pragma once

#include "db/ConsensusStorage.hpp"
#include "db/RedisDB.hpp"
#include "node/src/Buffer.hpp"
#include "node/src/io/HeaderSerializer.hpp"

#include "SlotContext.hpp"

namespace slot {
class PendingBlockSaver {
public:
  PendingBlockSaver(SlotContext &context, ::db::RedisDB &redis, Buffer &buffer,
                    ::db::ConsensusStorage &consensusStorage);

  void save();

private:
  void saveBlockToDb();
  void saveHeader();

  SlotContext &context;
  ::db::RedisDB &redis;
  Buffer &buffer;
  ::db::ConsensusStorage &consensusStorage;
  ::io::HeaderSerializer headerSerializer{};
};
} // namespace slot