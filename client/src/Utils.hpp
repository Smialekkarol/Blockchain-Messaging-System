#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

std::vector<std::string> tokenizeBySpace(const std::string &command);
std::string toLower(const std::string &str);