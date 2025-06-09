
#pragma once
#include <string>
#include <vector>
#include <random>

namespace WallpaperRandomGetter {
    extern int current_index;
    extern std::vector<int> indices;
    extern std::mt19937 rng;

    void initialize();
    void refresh();
    void reset();
    std::string get_next_wallpaper();
    std::string get_previous_wallpaper();
};

namespace WallpaperOrderGetter {
    extern int current_index;

    void initialize();
    void refresh();
    void reset();
    std::string get_next_wallpaper();
    std::string get_previous_wallpaper();
};
