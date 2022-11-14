#include <gtest/gtest.h>
#include <memory>

#include "../Timer.hpp"

namespace testing {
class TestTimer : public ::testing::Test {
public:
  void SetUp() { timer = std::make_shared<::common::utils::Timer>(); }

  void TearDown() { timer->stop(); }

  std::shared_ptr<::common::utils::Timer> timer;
};

TEST_F(TestTimer, shouldThrowExceptionOnDoubleSetTimeout) {
  timer->setTimeout([]() {}, 1);

  EXPECT_ANY_THROW(timer->setTimeout([]() {}, 1));
}

TEST_F(TestTimer, shouldThrowExceptionOnDoubleSetInterval) {
  timer->setInterval([]() {}, 1);

  EXPECT_ANY_THROW(timer->setInterval([]() {}, 1));
}

TEST_F(TestTimer, shouldThrowExceptionOnDoubleTimerActivation) {
  timer->setTimeout([]() {}, 1);

  EXPECT_ANY_THROW(timer->setInterval([]() {}, 1));
}

TEST_F(TestTimer, shouldStopTimeoutBeforeGivenTime) {
  const int interval{100};
  int value{0};

  timer->setTimeout([&value]() { value++; }, interval);
  std::this_thread::sleep_for(std::chrono::milliseconds(interval / 2));
  timer->stop();
  std::this_thread::sleep_for(std::chrono::milliseconds(interval * 2));

  EXPECT_EQ(value, 0);
}

TEST_F(TestTimer, shouldIncrementValueAfter10Miliseconds) {
  const int interval{10};
  int value{0};

  timer->setTimeout([&value]() { value++; }, interval);
  std::this_thread::sleep_for(std::chrono::milliseconds(interval * 2));

  EXPECT_EQ(value, 1);
}

TEST_F(TestTimer, shouldSetTimeoutTwiceSuccesfully) {
  const int interval{10};
  int value{0};

  timer->setTimeout([&value]() { value++; }, interval);
  std::this_thread::sleep_for(std::chrono::milliseconds(interval * 2));
  timer->setTimeout([&value]() { value++; }, interval);
  std::this_thread::sleep_for(std::chrono::milliseconds(interval * 2));

  EXPECT_EQ(value, 2);
}

TEST_F(TestTimer, shouldStopIntervalBeforeGivenTime) {
  const int interval{100};
  int value{0};

  timer->setInterval([&value]() { value++; }, interval);
  std::this_thread::sleep_for(std::chrono::milliseconds(interval / 2));
  timer->stop();
  std::this_thread::sleep_for(std::chrono::milliseconds(interval * 2));

  EXPECT_EQ(value, 0);
}

TEST_F(TestTimer, shouldIncrementValueThreeTimesAfter300Miliseconds) {
  const int interval{100};
  int value{0};

  timer->setInterval([&value]() { value++; }, interval);
  std::this_thread::sleep_for(std::chrono::milliseconds((interval * 3) + 30));
  timer->stop();

  EXPECT_EQ(value, 3);
}

TEST_F(TestTimer,
       shouldSetIntervalAndSetTimeoutSuccessfullyWithSimulatedThimeShift) {
  const int interval{100};
  int value{0};

  timer->setInterval([&value]() { value++; }, interval);
  std::this_thread::sleep_for(std::chrono::milliseconds((interval * 3) + 10));
  timer->stop();
  std::this_thread::sleep_for(std::chrono::milliseconds((interval) + 10));
  timer->setTimeout([&value]() { value++; }, interval);
  std::this_thread::sleep_for(std::chrono::milliseconds((interval) + 10));
  timer->stop();

  EXPECT_EQ(value, 4);
}

TEST_F(TestTimer, shouldSetIntervalThreeTimesSuccessfully) {
  const int interval{100};
  int value{0};

  timer->setInterval([&value]() { value++; }, interval);
  std::this_thread::sleep_for(std::chrono::milliseconds((interval * 3) + 30));
  timer->stop();
  timer->setInterval([&value]() { value++; }, interval);
  std::this_thread::sleep_for(std::chrono::milliseconds((interval * 3) + 30));
  timer->stop();
  timer->setInterval([&value]() { value++; }, interval);
  std::this_thread::sleep_for(std::chrono::milliseconds((interval * 3) + 30));
  timer->stop();

  EXPECT_EQ(value, 9);
}

TEST_F(TestTimer, shouldSetIntervalThreeTimesAndSetTimeoutSuccessfully) {
  const int interval{100};
  int value{0};

  timer->setInterval([&value]() { value++; }, interval);
  std::this_thread::sleep_for(std::chrono::milliseconds((interval * 3) + 30));
  timer->stop();
  timer->setInterval([&value]() { value++; }, interval);
  std::this_thread::sleep_for(std::chrono::milliseconds((interval * 3) + 30));
  timer->stop();
  timer->setInterval([&value]() { value++; }, interval);
  std::this_thread::sleep_for(std::chrono::milliseconds((interval) + 30));
  timer->stop();

  EXPECT_EQ(value, 7);
}

TEST_F(TestTimer, shouldStopIntervalAfterEndOfLifeOfTimer) {
  const int interval{100};
  int value{0};

  {
    ::common::utils::Timer t{};
    t.setInterval([&value]() { value++; }, interval);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds((interval * 2) + 10));

  EXPECT_EQ(value, 0);
}
} // namespace testing
