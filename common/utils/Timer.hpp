#pragma once
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

template <typename Callback> class Timer {
public:
  void setTimeout(Callback callback, int delay) {
    active = true;
    std::thread t([=]() {
      if (!active.load()) {
        return;
      }
      std::cout<<delay<<std::endl; 
      std::this_thread::sleep_for(std::chrono::milliseconds(delay));
      if (!active.load()) {
        return;
      }
      callback();
    });
    t.detach();
  }

  void setInterval(Callback callback, int interval) {
    active = true; 
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

  void stop() { active = false; }

private:
  std::atomic<bool> active{false};
};
