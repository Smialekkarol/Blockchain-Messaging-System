#pragma once

#include <functional>
#include <unordered_map>

#include "ConnectionData.hpp"

namespace io {
class Channel {
public:
  Channel(const ::io::ConnectionData &connectionData,
          const std::string &type = "DEFAULT");
  Channel(const ::io::Channel &other) = delete;
  Channel &operator=(const ::io::Channel &other) = delete;
  Channel(::io::Channel &&other) = default;
  Channel &operator=(::io::Channel &&other) = default;
  ~Channel() = default;

  void consume(std::function<void(const AMQP::Message &msg, uint64_t tag,
                                  bool redelivered)>
                   consumeHandler);
  void publish(const std::string &message);

  std::string type{};
  std::string node{};
  std::string queue{};

protected:
  virtual void setupChannelErrorHandling();
  virtual void declareQueue();

  AMQP::TcpChannel channel;
  std::function<void(const char *message)> channelErrorHandler{nullptr};
  std::function<void(const char *message)> queueErrorHandler{nullptr};
};
} // namespace io