#include "Text.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace common::utils {
std::string Text::toLower(const std::string &input) {
  std::string lower{input};
  std::transform(lower.begin(), lower.end(), lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return lower;
}

std::vector<std::string> Text::tokenizeBySpace(const std::string &input) {
  std::vector<std::string> args;
  std::string tmp;
  auto iss = std::istringstream(input);
  while (iss >> tmp) {
    args.push_back(tmp);
  }
  return args;
}
} // namespace common::utils