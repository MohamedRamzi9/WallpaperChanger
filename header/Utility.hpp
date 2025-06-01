
#pragma once

#include <string>
#include <vector>
#include "rmz_type.hpp"
#include "../header/WallpaperChanger.hpp"


std::vector<std::string> split_string(const std::string& str, char delimiter);
std::string join_string(const std::vector<std::string>& vec, const std::string& delimiter);
