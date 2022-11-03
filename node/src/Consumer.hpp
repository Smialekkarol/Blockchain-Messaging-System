#pragma once

#include "common/NodeConfiguration.hpp"

class Consumer {
public:
  static void run(const common::NodeConfiguration &config);

private:
  static void main(const common::NodeConfiguration &config);
};