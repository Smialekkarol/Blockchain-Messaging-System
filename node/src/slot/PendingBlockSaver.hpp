#pragma once

#include "db/RedisDB.hpp"
#include "node/src/Buffer.hpp"
#include "node/src/io/HeaderSerializer.hpp"

#include "SlotContext.hpp"

namespace slot {
class PendingBlockSaver {
public:
  PendingBlockSaver(SlotContext &context, ::db::RedisDB &redis, Buffer &buffer);

  void save();

private:
  void saveBlockToDb();
  void saveHeader();

  SlotContext &context;
  ::db::RedisDB &redis;
  Buffer &buffer;
  ::io::HeaderSerializer headerSerializer{};
};
} // namespace slot