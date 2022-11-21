#pragma once

#include <static_mock/Mock.hpp>

#if !USE_ORIGINAL_CLASS(Buffer)

#include "gmock/gmock.h"
#include <gtest/gtest.h>

#include <optional>
#include <vector>

#include "common/itf/Block.hpp"
#include "common/itf/Message.hpp"

class BufferMock {
public:
  MOCK_METHOD(void, push, (const std::vector<::common::itf::Message> &message),
              ());
  MOCK_METHOD(void, push, (const ::common::itf::Message &message), ());
  MOCK_METHOD(void, save, (), ());
  MOCK_METHOD(std::optional<::common::itf::Block>, pop, (), ());
};

STATIC_MOCK_CLASS(BufferConstructor) {
public:
  MOCK_METHOD(std::shared_ptr<BufferMock>, construct, (), ());
};

class MOCK_FACADE(Buffer) : public ::testing::MockBase<BufferMock> {
public:
  Buffer() { this->setMock(STATIC_MOCK(BufferConstructor).construct()); }
  Buffer(const Buffer &other) = delete;
  Buffer(Buffer &&other) = delete;
  Buffer &operator=(const Buffer &other) = delete;
  Buffer &operator=(Buffer &&other) = delete;
  ~Buffer() = default;

  WRAP_METHOD(push);
  WRAP_METHOD(save);
  WRAP_METHOD(pop);
};

#endif
