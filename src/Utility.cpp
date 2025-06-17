
#include "WallpaperManager.hpp"
#include "Utility.hpp"

#include <algorithm>
#include <ranges>
#include <cctype>

std::vector<std::string> split_string(const std::string& str, char delimiter) {
    std::vector<std::string> result;

    for (auto &&part : std::views::split(str, delimiter)) {
        result.emplace_back(part.begin(), part.end());
    }

    return result;
} 

std::string join_string(const std::vector<std::string>& vec, const std::string& delimiter) {
    if (vec.empty()) return "";
    std::string result = vec[0];
    for (size_t i = 1; i < vec.size(); ++i) {
        result += delimiter + vec[i];
    }
    return result;
}

void left_trim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [](unsigned char ch) { return !std::isspace(ch); }));
}
void right_trim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
        [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}
void trim(std::string &s) {
    left_trim(s);
    right_trim(s);
}

std::pair<std::string, int> count_wallpapers_message(const std::string& folder) {
    int count = 0;
    std::string message;

    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (WallpaperManager::is_valid_wallpaper(entry.path())) {
            count++;
            if (count <= 5) {
                message += entry.path().string() + "\n";
            }
        }
    }

    if (count > 5) {
        message += "... and more\n";
    }

    return {message, count};
}