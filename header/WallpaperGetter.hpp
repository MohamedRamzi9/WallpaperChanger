
#pragma once
#include <string>
#include <vector>
#include <random>

struct WallpaperRandomGetter {
    static int current_index;
    static std::vector<int> indices;
    static std::mt19937 rng;

    static void initialize();
    static void refresh();
    static void reset();
    static std::string get_next_wallpaper();
    static std::string get_previous_wallpaper();
};

struct WallpaperOrderGetter {
    static int current_index;

    static void initialize();
    static void refresh();
    static void reset();
    static std::string get_next_wallpaper();
    static std::string get_previous_wallpaper();
};
