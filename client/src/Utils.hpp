#pragma once

#include <vector>
#include <string>

std::vector<std::string> tokenizeBySpace(const std::string &command);
std::string toLower(const std::string &str);
std::string readNodes(const std::string &configPath, const std::string &nodeName);