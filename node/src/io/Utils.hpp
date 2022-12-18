#pragma once

#include <string>
#include <utility>

namespace io {
constexpr const char *separator{"<@SEPARATOR#>"};
inline std::pair<std::string, std::string> split(const std::string &string) {
  const auto index = string.find(separator);
  return std::make_pair(string.substr(0, index),
                        string.substr(index + 13, string.length()));
}

inline std::string merge(const std::string &s1, const std::string &s2) {
  return s1 + separator + s2;
}
} // namespace io