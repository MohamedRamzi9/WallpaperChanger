
#pragma once

#include "WallpaperManager.hpp"

#include <string>
#include <vector>
#include <filesystem>

#include "rmz_type.hpp"


std::vector<std::string> split_string(const std::string& str, char delimiter);
std::string join_string(const std::vector<std::string>& vec, const std::string& delimiter);
void left_trim(std::string &s);
void right_trim(std::string &s);
void trim(std::string &s);
std::string count_wallpapers_message(const std::vector<WallpaperManager::wallpaper_type>& wallpapers);

