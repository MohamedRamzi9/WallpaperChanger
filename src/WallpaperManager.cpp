
#include "WallpaperManager.hpp"

#include <iostream>


namespace WallpaperManager {

	std::vector<std::string> folders;
	std::vector<std::string> wallpapers;
	std::vector<std::string> wallpaper_extensions = {".jpg", ".jpeg", ".png", ".bmp"};


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
	void remove_folder(const std::string& folder) {
		auto it = std::remove(folders.begin(), folders.end(), folder);
		if (it != folders.end()) {
			folders.erase(it, folders.end());
			load_all_wallpapers(); // Reload wallpapers after removing a folder
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
	bool is_valid_wallpaper(const std::filesystem::path& path) {
		std::string ext = path.extension().string();
		for (auto& c : ext) c = static_cast<char>(tolower(c));
		return std::any_of(wallpaper_extensions.begin(), wallpaper_extensions.end(), [&ext](const std::string& valid_ext) { return ext == valid_ext; });
	}

}