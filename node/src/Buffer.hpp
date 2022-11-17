#pragma once

#include "common/itf/Block.hpp"
#include "common/itf/Message.hpp"

#include <deque>
#include <mutex>

class Buffer {
public:
  Buffer() = default;
  Buffer(const Buffer &other) = delete;
  Buffer(Buffer &&other) = delete;
  Buffer &operator=(const Buffer &other) = delete;
  Buffer &operator=(Buffer &&other) = delete;
  ~Buffer() = default;

  void push(const ::common::itf::Message &message);
  void save();
  ::common::itf::Block pop();

private:
  std::vector<::common::itf::Message> messages{};
  std::deque<::common::itf::Block> blocks{};
  std::mutex mutex{};
};