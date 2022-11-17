#include "Buffer.hpp"

void Buffer::push(const ::common::itf::Message &message) {
  std::scoped_lock lock(mutex);
  messages.push_back(message);
}

void Buffer::save() {
  std::scoped_lock lock(mutex);
  blocks.push_back(std::move(messages));
  messages = {};
}

::common::itf::Block Buffer::pop() {
  std::scoped_lock lock(mutex);
  const auto &block = blocks.front();
  blocks.pop_front();
  return block;
}
