#include <gtest/gtest.h>
#include <memory>

#include "../Buffer.hpp"

namespace testing {
class TestBuffer : public ::testing::Test {
public:
  void SetUp() { buffer = std::make_shared<Buffer>(); }

  std::shared_ptr<Buffer> buffer{};
};

TEST_F(TestBuffer, popBlockWithEmptyBuffer) {
  buffer->save();
  const auto block = buffer->pop();

  EXPECT_FALSE(block.has_value());
}

TEST_F(TestBuffer, popBlockWithTwoSepartePushCalls) {
  ::common::itf::Message message1{1, "data1", "author1"};
  ::common::itf::Message message2{2, "data2", "author2"};

  buffer->push(message1);
  buffer->push(message2);
  buffer->save();
  const auto block = buffer->pop();

  EXPECT_TRUE(block.has_value());
  EXPECT_EQ(block.value().data.size(), 2);
}

TEST_F(TestBuffer, popBlockPushedWithVectorOfMessages) {
  ::common::itf::Message message1{1, "data1", "author1"};
  ::common::itf::Message message2{2, "data2", "author2"};
  std::vector<::common::itf::Message> messages{message1, message2};

  buffer->push(messages);
  buffer->save();
  const auto block = buffer->pop();

  EXPECT_TRUE(block.has_value());
  EXPECT_EQ(block.value().data.size(), 2);
}
} // namespace testing