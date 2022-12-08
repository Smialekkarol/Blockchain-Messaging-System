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

std::vector<std::string> Text::splitBySeparator(const std::string data, const char * separator) {
  typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

  boost::char_separator<char> at(separator);
  tokenizer dataTokens(data, at);
  std::vector<std::string> dataAttributes{};

  BOOST_FOREACH (std::string const &token, dataTokens) {
    dataAttributes.push_back(token);
  }

  return dataAttributes;
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