#pragma once

#include <string>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

namespace common::utils {
class Text {
public:
  static std::string toLower(const std::string &input);
  static std::vector<std::string> tokenizeBySpace(const std::string &input);
  static std::vector<std::string> splitBySeparator(const std::string data, const char * separator);
};
} // namespace common::utils