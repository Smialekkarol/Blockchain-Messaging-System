#pragma once

// clang-format off
// #include <static_mock/Mock.hpp>
// #include INCLUDE_TESTABLE_MOCK("ut/mocks/TimerMock.hpp")
// #if USE_ORIGINAL_CLASS(Timer)
// clang-format on

#include <atomic>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace common::utils {
class Timer {
  class TimerAlreadyActive : public std::exception {};

public:
  Timer() = default;

  Timer(const Timer &other) = delete;
  Timer(Timer &&other) = delete;
  Timer &operator=(const Timer &other) = delete;
  Timer &operator=(Timer &&other) = delete;

  ~Timer() {
    if (active.load()) {
      stop();
      std::this_thread::sleep_for(
          std::chrono::milliseconds(static_cast<int>(1.1 * time)));
    }
  }

  template <typename Callback>
  void setTimeout(Callback callback, const int delay) {
    throwErrorIfTimerAlreadyStarted();
    waitForPendingCallbacks();
    updateTimerState(delay);
    std::thread t([=]() {
      if (!active.load()) {
        counter.fetch_sub(1);
        return;
      }
      std::cout<<delay<<std::endl; 
      std::this_thread::sleep_for(std::chrono::milliseconds(delay));
      if (!active.load()) {
        counter.fetch_sub(1);
        return;
      }
      callback();
      counter.fetch_sub(1);
      stop();
    });
    t.detach();
  }

  template <typename Callback>
  void setInterval(Callback callback, const int interval) {
    throwErrorIfTimerAlreadyStarted();
    waitForPendingCallbacks();
    updateTimerState(interval);
    std::thread t([=]() {
      while (active.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        if (!active.load()) {
          counter.fetch_sub(1);
          return;
        }
        callback();
      }
      counter.fetch_sub(1);
    });
    t.detach();
  }

  void stop() { active.store(false); }

private:
  void throwErrorIfTimerAlreadyStarted() const {
    if (active.load()) {
      throw TimerAlreadyActive{};
    }
  }

  void waitForPendingCallbacks() const {
    while (counter.load() != 0) {
      std::this_thread::sleep_for(
          std::chrono::milliseconds(static_cast<int>(1.1 * time)));
    }
  }

  void updateTimerState(const int time) {
    active.store(true);
    counter.fetch_add(1);
    this->time = time;
  }

  std::atomic<int> counter{0};
  std::atomic<bool> active{false};
  int time{0};
};
} // namespace common::utils

// #endif
