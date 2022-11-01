#include "Utils.hpp"

std::vector<std::string> tokenizeBySpace(const std::string &command) {
  std::vector<std::string> args;
  std::string tmp;
  auto iss = std::istringstream(command);
  while (iss >> tmp) {
    args.push_back(tmp);
  }
  return args;
}

std::string toLower(const std::string &str) {
  std::string lower{str};
  std::transform(lower.begin(), lower.end(), lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return lower;
}
