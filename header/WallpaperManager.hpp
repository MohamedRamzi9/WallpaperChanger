
#pragma once
#include <string>
#include <vector>


namespace WallpaperManager {
    extern std::vector<std::string> folders;
    extern std::vector<std::string> wallpapers;
    
    void initialize();
    void add_folder(const std::string& folder);
    bool empty();
    void load_wallpapers(const std::string& folder);
    void load_all_wallpapers();

    int get_wallpaper_count();
    std::vector<std::string>& get_folders();
    std::string get_wallpaper(int index);
};