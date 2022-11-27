#include "common/itf/Constants.hpp"

#include "SlotHandler.hpp"

namespace slot {

SlotHandler::SlotHandler(::db::RedisDB &redis_, Buffer &buffer_, Consumer& consumer_)
    : redis{redis_}, buffer{buffer_}, consumer{consumer_} {}

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
    blockIndex = redis.add(this->block, ::common::itf::DEFAULT_BLOCKAIN);
  } else {
    shouldCallNextHandler = false;
  }
};

void SlotHandler::saveCompleteBlock() {
  block.state = ::common::itf::BlockState::COMPLETED;
  redis.update(block, blockIndex -1, ::common::itf::DEFAULT_BLOCKAIN);
}

void SlotHandler::publishBlock(){
  consumer.publish(block);
}
} // namespace slot
