#pragma once

#include <string>
#include <vector>

namespace common::utils {
class Text {
public:
  static std::string toLower(const std::string &input);
  static std::vector<std::string> tokenizeBySpace(const std::string &input);
};
} // namespace common::utils