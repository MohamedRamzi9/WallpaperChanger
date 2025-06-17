
#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "rmz_type.hpp"


std::vector<std::string> split_string(const std::string& str, char delimiter);
std::string join_string(const std::vector<std::string>& vec, const std::string& delimiter);
void left_trim(std::string &s);
void right_trim(std::string &s);
void trim(std::string &s);
std::pair<std::string, int> count_wallpapers_message(const std::string& folder);

