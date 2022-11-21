#include "Buffer.hpp"

void Buffer::push(const std::vector<::common::itf::Message> &messages) {
  std::scoped_lock lock(mutex);
  this->messages.insert(this->messages.begin(), messages.begin(),
                        messages.end());
}

void Buffer::push(const ::common::itf::Message &message) {
  std::scoped_lock lock(mutex);
  messages.push_back(message);
}

void Buffer::save() {
  std::scoped_lock lock(mutex);
  if (messages.size() > 0) {
    blocks.push_back(std::move(messages));
    messages = {};
  }
}

std::optional<::common::itf::Block> Buffer::pop() {
  std::optional<::common::itf::Block> block{};
  std::scoped_lock lock(mutex);
  if (blocks.size() > 0) {
    block.emplace(blocks.front());
    blocks.pop_front();
  }
  return block;
}
