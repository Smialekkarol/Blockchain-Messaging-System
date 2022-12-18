#include "common/itf/Constants.hpp"

#include "SlotHandler.hpp"

namespace slot {

SlotHandler::SlotHandler(::db::RedisDB &redis_, Buffer &buffer_,
                         Consumer &consumer_, ::io::ChannelStore &channelStore_)
    : redis{redis_}, buffer{buffer_}, consumer{consumer_}, channelStore{
                                                               channelStore_} {}

void SlotHandler::handle() {
  shouldCallNextHandler = true;
  for (const auto handler : handlers) {
    if (!shouldCallNextHandler) {
      break;
    }
    handler();
  }
}

void SlotHandler::savePendingBlock() {
  buffer.save();
  if (const auto b = buffer.pop(); b.has_value()) {
    this->block = b.value();
    const auto blockchainSize =
        redis.add(this->block.value(), ::common::itf::DEFAULT_BLOCKAIN);
    blockIndex = blockchainSize - 1;
  } else {
    ::common::itf::Block emptyBlock{};
    const auto blockchainSize =
        redis.add(emptyBlock, ::common::itf::DEFAULT_BLOCKAIN);
    blockIndex = blockchainSize - 1;
  }
};

void SlotHandler::waitForNodesInspection() {
  // TODO: Provide waiting implementation
}

void SlotHandler::removePendingBlock() {
  if (block.has_value()) {
    return;
  }
  // TODO: this setting shouldCallNextHandler is temporary, as in future we will
  // need to get information from other nodes.
  shouldCallNextHandler = false;
  const auto &b = redis.getByIndex(blockIndex, ::common::itf::DEFAULT_BLOCKAIN);
  redis.remove(b.value(), ::common::itf::DEFAULT_BLOCKAIN);
}

void SlotHandler::saveCompleteBlock() {
  block.value().state = ::common::itf::BlockState::COMPLETED;
  redis.update(block.value(), blockIndex, ::common::itf::DEFAULT_BLOCKAIN);
}

void SlotHandler::publishBlock() { consumer.publish(block.value()); }
} // namespace slot
