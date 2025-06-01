
#pragma once
#include <string>
#include <vector>


struct WallpaperManager {
    static std::vector<std::string> folders;
    static std::vector<std::string> wallpapers;
    
    static void initialize();
    static void add_folder(const std::string& folder);
    static bool empty();
    static void load_wallpapers(const std::string& folder);
    static void load_all_wallpapers();

    static int get_wallpaper_count();
    static std::vector<std::string>& get_folders();
    static std::string get_wallpaper(int index);
};