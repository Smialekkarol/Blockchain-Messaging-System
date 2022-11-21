#pragma once

// clang-format off
#include <static_mock/Mock.hpp>
#include INCLUDE_TESTABLE_MOCK("ut/mocks/BufferMock.hpp")
#if USE_ORIGINAL_CLASS(Buffer)
// clang-format on

#include <deque>
#include <mutex>
#include <optional>

#include "common/itf/Block.hpp"
#include "common/itf/Message.hpp"

class Buffer {
public:
  Buffer() = default;
  Buffer(const Buffer &other) = delete;
  Buffer(Buffer &&other) = delete;
  Buffer &operator=(const Buffer &other) = delete;
  Buffer &operator=(Buffer &&other) = delete;
  ~Buffer() = default;

  void push(const std::vector<::common::itf::Message> &messages);
  void push(const ::common::itf::Message &message);
  void save();
  std::optional<::common::itf::Block> pop();

private:
  std::vector<::common::itf::Message> messages{};
  std::deque<::common::itf::Block> blocks{};
  std::mutex mutex{};
};
#endif
