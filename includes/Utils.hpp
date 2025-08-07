// Utils.hpp
#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <cctype>

std::string         toLower(const std::string& str);
bool                isValidNickname(const std::string& nick);
std::string         getCurrentDate();

#endif
