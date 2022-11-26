#include <gtest/gtest.h>

#include <memory>
#include <thread>

#include "../TimerWheel.hpp"

namespace testing {
class TestTimerWheel : public ::testing::Test {
public:
  void SetUp() {
    EXPECT_CALL(STATIC_MOCK(::common::utils::TimeMock),
                getTimeToTheNearestSlot())
        .WillRepeatedly(Return(0));

    timerMock = std::make_shared<::common::utils::TimerMock>();
    EXPECT_CALL(STATIC_MOCK(::common::utils::TimerConstructor), construct())
        .Times(1)
        .WillRepeatedly(Return(timerMock));

    timerWheel = std::make_shared<::slot::TimerWheel>();
  }

  void TearDown() { EXPECT_CALL(*timerMock, stop()).Times(1); }

  void callTimes(std::function<void()> callback, const int iterations) {
    for (int i = 0; i < iterations; i++) {
      callback();
    }
  }

  std::shared_ptr<::common::utils::TimerMock> timerMock{};
  std::shared_ptr<::slot::TimerWheel> timerWheel{};
};

TEST_F(TestTimerWheel, shouldCallTwoCallbacksForTwoSlots) {
  int counter{0};
  EXPECT_CALL(*timerMock, setIntervalVoid(_, 500))
      .Times(1)
      .WillRepeatedly(Invoke(
          [&](auto callback, const int interval) { callTimes(callback, 2); }));

  timerWheel->subscribe([&]() { counter++; });
  timerWheel->subscribe([&]() { counter++; });
  timerWheel->start();

  EXPECT_EQ(counter, 4);
}

TEST_F(TestTimerWheel, shouldUnsubscribeOneSubscription) {
  int counter{0};
  std::function<void()> capturedCallback{nullptr};
  EXPECT_CALL(*timerMock, setIntervalVoid(_, 500))
      .Times(1)
      .WillRepeatedly(Invoke([&](auto callback, const int interval) {
        capturedCallback = callback;
      }));

  ::slot::TimerWheel::Subscription subscription =
      timerWheel->subscribe([&]() { counter++; });
  timerWheel->subscribe([&]() { counter++; });
  timerWheel->start();
  capturedCallback();
  subscription.unsubscribe();
  capturedCallback();

  EXPECT_EQ(counter, 3);
}
} // namespace testing