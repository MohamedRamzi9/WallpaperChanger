
#include "WallpaperManager.hpp"

#include "rmz_print.hpp"


namespace WallpaperManager {
	
	std::vector<wallpaper_type> wallpaper_extensions = {".jpg", ".jpeg", ".png", ".bmp"};

	folder_type::folder_type(const std::string& path) : path(path) {}
	void folder_type::add_wallpaper(const wallpaper_type& wallpaper)  { wallpapers.push_back(wallpaper); }
	void folder_type::load_wallpapers() {
		wallpapers.clear();
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			if (entry.is_regular_file()) {
				std::string path = entry.path().string();
				std::string ext = entry.path().extension().string();

				// Convert extension to lowercase for comparison
				for (auto& c : ext) c = static_cast<char>(tolower(c));

				if (std::find(wallpaper_extensions.begin(), wallpaper_extensions.end(), ext) != wallpaper_extensions.end()) {
					wallpapers.push_back(path);
					rmz::println("folder {}, size {}", path, size());
				}
			}
		}
	}
	const wallpaper_type& folder_type::get_wallpaper(int index) const { return wallpapers[index]; }
	const std::vector<wallpaper_type>& folder_type::get_wallpapers() const { return wallpapers; }
	int folder_type::size() const { return wallpapers.size(); }


	std::vector<folder_type> folders;
	int wallpaper_count = 0;	


	void initialize() { folders.emplace_back("default"); }
	void add_wallpaper(const wallpaper_type& wallpaper) {
		folders[0].add_wallpaper(wallpaper);
		wallpaper_count++;
	}
	const folder_type& add_folder(const std::string& path) {
		folder_type& folder = folders.emplace_back(path);
		folder.load_wallpapers();
		wallpaper_count += folder.size();
		return folder;
	}
	void remove_folder(int index) {
		auto it = folders.begin() + index;
		wallpaper_count -= it->size();
		folders.erase(it);
	}
	void remove_folder(const std::string& folder) {
		auto it = std::find_if(folders.begin(), folders.end(), [&folder](const folder_type& f) { return f.path == folder; });
		wallpaper_count -= it->size();
		folders.erase(it);
	}
	bool empty() { return wallpaper_count == 0; }
	void refresh() {
		wallpaper_count = folders[0].size();
		for (auto it = folders.begin() + 1; it != folders.end(); ++it) {
			it->load_wallpapers();
			wallpaper_count += it->size();
		}
	}

	int get_wallpaper_count() { return wallpaper_count; }
	const std::vector<wallpaper_type>& get_alone_wallpapers() { return folders[0].get_wallpapers(); }
	std::vector<folder_type>& get_folders() { return folders; }
	const wallpaper_type& get_wallpaper(int index) { 
		int i = 0;
		for (const auto& folder : folders) {
			rmz::println("Checking folder {} at i {}, size {}", folder.path, i, folder.size());
			if (index < i + folder.size()) {
				rmz::println("Getting wallpaper at index {} from folder {}", index, folder.path);
				return folder.get_wallpaper(index - i);
			}
			i += folder.size();
		}
		rmz::println("i {}, index {}, wallpaper_count {}", i, index, wallpaper_count);
	}
	bool is_valid_wallpaper(const std::filesystem::path& path) {
		std::string ext = path.extension().string();
		for (auto& c : ext) c = static_cast<char>(tolower(c));
		return std::any_of(wallpaper_extensions.begin(), wallpaper_extensions.end(), [&ext](const std::string& valid_ext) { return ext == valid_ext; });
	}
	bool is_valid_folder(const std::filesystem::path& path) {
		return std::filesystem::exists(path) && std::filesystem::is_directory(path);
	}

}