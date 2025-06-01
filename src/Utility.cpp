
#include "Utility.hpp"

#include <algorithm>
#include <ranges>


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
