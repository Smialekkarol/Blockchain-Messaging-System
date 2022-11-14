#pragma once

#include <static_mock/Mock.hpp>

#if !USE_ORIGINAL_CLASS(Timer)

#include "gmock/gmock.h"
#include <functional>
#include <gtest/gtest.h>

class TimerMock {
public:
  MOCK_METHOD(void, stop, (), ());
  MOCK_METHOD(void, setIntervalVoid, (std::function<void()>, const int), ());
  MOCK_METHOD(void, setTimeoutVoid, (std::function<void()>, const int), ());

  template <typename Callback>
  void setInterval(Callback callback, const int interval);

  template <typename Callback>
  void setTimeout(Callback callback, const int delay);
};

template <>
inline void TimerMock::setInterval<int>(std::function<void()> callback,
                                        const int interval) {
  return setIntervalVoid(callback, interval);
}

template <>
inline void TimerMock::setTimeout<int>(std::function<void()> callback,
                                       const int delay) {
  return setTimeoutVoid(callback, delay);
}

STATIC_MOCK_CLASS(TimerConstructor) {
public:
  MOCK_METHOD(std::shared_ptr<TimerMock>, construct, (), ());
};

class MOCK_FACADE(Timer) : public ::testing::MockBase<TimerMock> {
public:
  Timer() { this->setMock(STATIC_MOCK(TimerConstructor).construct()); }

  WRAP_METHOD_TEMPLATE(setInterval);
  WRAP_METHOD_TEMPLATE(setTimeout);
  WRAP_METHOD(stop);
};

#endif
