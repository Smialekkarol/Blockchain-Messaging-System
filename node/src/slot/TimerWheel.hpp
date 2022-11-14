#pragma once

// clang-format off
#include <static_mock/Mock.hpp>
#include INCLUDE_TESTABLE_MOCK("ut/mocks/TimerWheelMock.hpp")
#if USE_ORIGINAL_CLASS(TimerWheel)
// clang-format on

#include "common/utils/Timer.hpp"
#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace slot {

class TimerWheel {
  friend class Subscription;

public:
  class Subscription {
    friend struct HashFunction;
    friend class TimerWheel;

  public:
    Subscription(Subscription &&other) = default;
    Subscription &operator=(Subscription &&other) = default;

    Subscription(const Subscription &other) = default;
    Subscription &operator=(const Subscription &other) = delete;

    ~Subscription() = default;

    void unsubscribe() { timerWheel->unsubscribe(*this); };

    bool operator==(const Subscription &other) const { return id == other.id; }

    struct HashFunction {
      std::size_t operator()(const Subscription &s) const {
        return std::hash<int>()(s.id);
      }
    };

  private:
    Subscription(const uint64_t id_, TimerWheel *timerWheel_)
        : id{id_}, timerWheel{std::move(timerWheel_)} {};

    uint64_t id{0};
    TimerWheel *timerWheel{nullptr};
  };

public:
  TimerWheel(const int slot_ = 500) : slot{slot_} {};
  TimerWheel(const TimerWheel &other) = delete;
  TimerWheel(TimerWheel &&other) = delete;
  TimerWheel &operator=(const TimerWheel &other) = delete;
  TimerWheel &operator=(TimerWheel &&other) = delete;
  ~TimerWheel() = default;

  void start() {
    timer.setInterval(
        [&]() {
          std::scoped_lock lock(mutex);
          for (const auto [_, c] : callbacks) {
            c();
          }
        },
        slot);
  };

  void stop() {
    std::scoped_lock lock(mutex);
    timer.stop();
  };

  Subscription subscribe(std::function<void()> callback) {
    std::scoped_lock lock(mutex);
    Subscription key{counter, this};
    callbacks.emplace(key, callback);
    Subscription subscription{counter, this};
    counter++;
    return subscription;
  }

private:
  void unsubscribe(const Subscription &subscription) {
    std::scoped_lock lock(mutex);
    callbacks.erase(subscription);
  };

  int slot{500};
  ::common::utils::Timer timer{};
  std::unordered_map<Subscription, std::function<void()>,
                     Subscription::HashFunction>
      callbacks{};
  uint64_t counter{0};
  std::mutex mutex{};
};

} // namespace slot

#endif
