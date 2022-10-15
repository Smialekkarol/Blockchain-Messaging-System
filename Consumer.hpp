#pragma once

#include "NodeConfiguration.hpp"

class Consumer {
public:
  static void run(const NodeConfiguration &config);

private:
  static void main(const NodeConfiguration &config);
};