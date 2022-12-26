#include <spdlog/spdlog.h>

#include "Channel.hpp"

namespace io {

Channel::Channel(const ::io::ConnectionData &connectionData,
                 const ChannelType type)
    : node{connectionData.node}, type{type},
      queue{node + "_" + std::to_string(static_cast<std::uint16_t>(type))},
      channel{connectionData.connection.get()} {
  setupChannelErrorHandling();
  declareQueue();
}

void Channel::consume(std::function<void(const AMQP::Message &msg, uint64_t tag,
                                         bool redelivered)>
                          consumeHandler) {
  if (consumeHandler == nullptr) {
    // TODO: Throw error
    spdlog::error("Channel[{}] error: nullptr consume callback passed", queue);
    return;
  }

  channel.consume(queue, AMQP::noack)
      .onReceived(
          [=](const AMQP::Message &msg, uint64_t tag, bool redelivered) {
            consumeHandler(msg, tag, redelivered);
          });
}

void Channel::publish(const std::string &message) {
  try {
    channel.publish("", queue, message);
  } catch (const std::exception &e) {
    spdlog::error("Error inside Channel[{}], reason: {}", queue, e.what());
  }
}

void Channel::setupChannelErrorHandling() {
  channel.onError([=](const char *message) {
    spdlog::error("Channel[{}] error: {}", queue, message);
    if (channelErrorHandler != nullptr) {
      channelErrorHandler(message);
    }
  });
}

void Channel::declareQueue() {
  spdlog::debug("Channel declaring queue: {}", queue);
  channel.declareQueue(queue, AMQP::durable).onError([=](const char *message) {
    spdlog::error("Channel[{}] failed to declare queue. Error: {}", queue,
                  message);
    if (queueErrorHandler != nullptr) {
      queueErrorHandler(message);
    }
  });
}
} // namespace io