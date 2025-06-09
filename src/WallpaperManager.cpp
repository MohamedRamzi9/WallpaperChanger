
#include "WallpaperManager.hpp"

#include <iostream>
#include <filesystem>


namespace WallpaperManager {

	std::vector<std::string> folders;
	std::vector<std::string> wallpapers;


	void initialize() {
		load_all_wallpapers();
	}
	void add_folder(const std::string& folder) {
		if (std::filesystem::exists(folder) && std::filesystem::is_directory(folder)) {
			folders.push_back(folder);
			load_wallpapers(folder);
		} else {
			std::cerr << "Folder does not exist or is not a directory: " << folder << '\n';
		}
	}
	bool empty() {
		return wallpapers.empty();
	}
	void load_wallpapers(const std::string& folder) {
		// try {
			for (const auto& entry : std::filesystem::directory_iterator(folder)) {
				if (entry.is_regular_file()) {
					std::string path = entry.path().string();
					std::string ext = entry.path().extension().string();

					// Convert extension to lowercase for comparison
					for (auto& c : ext) c = static_cast<char>(tolower(c));

					if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp") {
						wallpapers.push_back(path);
					}
				}
			}
		// } catch (const std::filesystem::filesystem_error& e) {
		// 	std::cerr << "Filesystem error: " << e.what() << '\n';
		// }
	}
	void load_all_wallpapers() {
		wallpapers.clear();
		for (const auto& folder : folders) {
			load_wallpapers(folder);
		}
	}
	int get_wallpaper_count() {
		return wallpapers.size();
	}
	std::vector<std::string>& get_folders() {
		return folders;
	}
	std::string get_wallpaper(int index) {
		return wallpapers[index];
	}


}