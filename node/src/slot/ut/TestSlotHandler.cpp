#include <gtest/gtest.h>
#include <memory>

#include "common/itf/Constants.hpp"

#include "../SlotHandler.hpp"

namespace testing {
class TestSlotHandler : public ::testing::Test {
public:
  void SetUp() {
    redisMock = std::make_shared<::db::RedisDBMock>();
    EXPECT_CALL(STATIC_MOCK(::db::RedisDBConstructor), construct())
        .Times(1)
        .WillRepeatedly(Return(redisMock));
    redis = std::make_shared<::db::RedisDB>();

    bufferMock = std::make_shared<BufferMock>();
    EXPECT_CALL(STATIC_MOCK(BufferConstructor), construct())
        .Times(1)
        .WillRepeatedly(Return(bufferMock));
    buffer = std::make_shared<Buffer>();

    slotHandler = std::make_shared<::slot::SlotHandler>(*redis, *buffer);
  }

  std::shared_ptr<::db::RedisDB> redis{};
  std::shared_ptr<Buffer> buffer{};
  std::shared_ptr<::db::RedisDBMock> redisMock{};
  std::shared_ptr<BufferMock> bufferMock{};
  std::shared_ptr<::slot::SlotHandler> slotHandler{};
};

TEST_F(TestSlotHandler, shouldNotAddPendingBlockWhenBufferIsEmpty) {
  std::optional<::common::itf::Block> block{};
  EXPECT_CALL(*bufferMock, save()).Times(1);
  EXPECT_CALL(*bufferMock, pop()).Times(1).WillOnce(Return(block));
  EXPECT_CALL(*redisMock, add(_, _)).Times(0);
  EXPECT_CALL(*redisMock, update(_, _, _)).Times(0);

  slotHandler->handle();
}

TEST_F(TestSlotHandler, shouldAddPendingAndCompletedBlock) {
  const long long index{1};
  ::common::itf::Message message1{1, "data1", "author1"};
  std::vector<::common::itf::Message> messages{message1};
  ::common::itf::Block pendingblock{1, messages};
  ::common::itf::Block completedBlock{1, messages};
  completedBlock.state = ::common::itf::BlockState::COMPLETED;
  EXPECT_CALL(*bufferMock, save()).Times(1);
  EXPECT_CALL(*bufferMock, pop())
      .Times(1)
      .WillOnce(Return(std::optional<::common::itf::Block>{pendingblock}));
  EXPECT_CALL(*redisMock, add(pendingblock, ::common::itf::DEFAULT_BLOCKAIN))
      .Times(1)
      .WillOnce(Return(index));
  EXPECT_CALL(*redisMock,
              update(completedBlock, index, ::common::itf::DEFAULT_BLOCKAIN))
      .Times(1);

  slotHandler->handle();
}
} // namespace testing