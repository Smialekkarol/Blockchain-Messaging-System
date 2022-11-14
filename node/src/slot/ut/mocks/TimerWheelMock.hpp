#pragma once

#include <static_mock/Mock.hpp>

#if !USE_ORIGINAL_CLASS(TimerWheel)

#include "gmock/gmock.h"
#include <functional>
#include <gtest/gtest.h>
#include <iostream>

namespace slot {

class TimerWheelMock {
public:
  MOCK_METHOD(void, stop, (), ());
  MOCK_METHOD(void, stop, (), ());
  MOCK_METHOD(void, Subscription, (std::function<void()>), ());
};

STATIC_MOCK_CLASS(TimerWheelConstructor) {
public:
  MOCK_METHOD(std::shared_ptr<TimerWheelMock>, construct, (const int slot_),
              ());
};

class MOCK_FACADE(TimerWheel) : public ::testing::MockBase<TimerWheelMock> {
public:
  class SubscriptionMock {
  public:
    MOCK_METHOD(void, unsubscribe, (), ());
  };

  STATIC_MOCK_CLASS(SubscriptionConstructor) {
  public:
    MOCK_METHOD(std::shared_ptr<SubscriptionMock>, construct,
                (const uint64_t id_, TimerWheel *timerWheel_), ());
  };

  class MOCK_FACADE(Subscription)
      : public ::testing::MockBase<SubscriptionMock> {
  public:
    Subscription(const uint64_t id_, TimerWheel *timerWheel_) {
      this->setMock(
          STATIC_MOCK(SubscriptionConstructor).construct(id_, timerWheel_));
    }

    Subscription(Subscription &&other) = default;
    Subscription &operator=(Subscription &&other) = default;

    Subscription(const Subscription &other) = default;
    Subscription &operator=(const Subscription &other) = delete;

    ~Subscription() = default;

    WRAP_METHOD(unsubscribe);
  };

  TimerWheel(const int slot_ = 500) {
    this->setMock(STATIC_MOCK(TimerWheelConstructor).construct(slot_));
  }
  TimerWheel(const TimerWheel &other) = delete;
  TimerWheel(TimerWheel &&other) = delete;
  TimerWheel &operator=(const TimerWheel &other) = delete;
  TimerWheel &operator=(TimerWheel &&other) = delete;
  ~TimerWheel() = default;

  WRAP_METHOD(start);
  WRAP_METHOD(stop);
  WRAP_METHOD(subscribe);
};
} // namespace slot

#endif
