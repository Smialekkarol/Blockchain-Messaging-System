#include <atomic>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace common::utils {
class Timer {
  class SetTimeoutAlreadyActive : public std::exception {};
  class SetIntervalAlreadyActive : public std::exception {};

public:
  template <typename Callback>
  void setTimeout(Callback callback, const int delay) {
    if (active) {
      throw SetTimeoutAlreadyActive{};
    }
    active.store(true);
    std::thread t([=]() {
      if (!active.load()) {
        return;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(delay));
      if (!active.load()) {
        return;
      }
      callback();
    });
    t.detach();
  }

  template <typename Callback>
  void setInterval(Callback callback, const int interval) {
    if (active) {
      throw SetIntervalAlreadyActive{};
    }
    active.store(true);
    std::thread t([=]() {
      while (active.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        if (!active.load()) {
          return;
        }
        callback();
      }
    });
    t.detach();
  }

  void stop() { active.store(false); }

private:
  std::atomic<bool> active{false};
};
} // namespace common::utils
