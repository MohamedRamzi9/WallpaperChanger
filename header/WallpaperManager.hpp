
#pragma once
#include <string>
#include <vector>
#include <filesystem>



namespace WallpaperManager {
    using wallpaper_type = std::string;
    struct folder_type {
		std::string path;
		std::vector<wallpaper_type> wallpapers;
        
		folder_type(const std::string& path);
        void add_wallpaper(const wallpaper_type& wallpaper);
        void load_wallpapers();
		const wallpaper_type& get_wallpaper(int index) const;
        const std::vector<wallpaper_type>& get_wallpapers() const;
		int size() const;
    };

    void initialize();
    void add_wallpaper(const wallpaper_type& wallpaper);
    const folder_type& add_folder(const std::string& folder);
    void remove_folder(int index);
    void remove_folder(const std::string& folder);
    bool empty();
    void refresh();

    int get_wallpaper_count();
    const std::vector<wallpaper_type>& get_alone_wallpapers();
    std::vector<folder_type>& get_folders();
    const wallpaper_type& get_wallpaper(int index);
    bool is_valid_wallpaper(const std::filesystem::path& path);
    bool is_valid_folder(const std::filesystem::path& path);

};