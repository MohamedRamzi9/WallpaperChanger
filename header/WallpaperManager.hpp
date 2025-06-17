
#pragma once
#include <string>
#include <vector>
#include <filesystem>



namespace WallpaperManager {
    extern std::vector<std::string> folders;
    extern std::vector<std::string> wallpapers;
    extern std::vector<std::string> wallpaper_extensions;
    
    void initialize();
    void add_folder(const std::string& folder);
    void remove_folder(const std::string& folder);
    bool empty();
    void load_wallpapers(const std::string& folder);
    void load_all_wallpapers();

    int get_wallpaper_count();
    std::vector<std::string>& get_folders();
    std::string get_wallpaper(int index);
    bool is_valid_wallpaper(const std::filesystem::path& path);

};