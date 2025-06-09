
#include "WallpaperGetter.hpp"
#include "WallpaperManager.hpp"

#include <algorithm>

namespace WallpaperRandomGetter {

	int current_index;
	std::vector<int> indices;
	std::mt19937 rng;

	void initialize() {
		rng = std::mt19937(std::random_device()());
		
		current_index = 0;
		indices.resize(WallpaperManager::get_wallpaper_count());
		std::iota(indices.begin(), indices.end(), 0);
		std::shuffle(indices.begin(), indices.end(), rng);
	}
	void refresh() {
		current_index = 0;
		indices.resize(WallpaperManager::get_wallpaper_count());
		std::iota(indices.begin(), indices.end(), 0);
		std::shuffle(indices.begin(), indices.end(), rng);
	}
	void reset() {
		current_index = 0;
	}
	std::string get_next_wallpaper() {
		current_index++;
		if (current_index >= indices.size()) 
			current_index = 0;
		return WallpaperManager::get_wallpaper(indices[current_index]);
	}
	std::string get_previous_wallpaper() {
		current_index--;
		if (current_index < 0) 
			current_index += indices.size();
		return WallpaperManager::get_wallpaper(indices[current_index]);
	}

}



namespace WallpaperOrderGetter {
	
	int current_index;

	void initialize() {
		current_index = 0;
	}
	void refresh() {}
	void reset() {
		current_index = 0;
	}
	std::string get_next_wallpaper() {
		current_index++;
		if (current_index >= WallpaperManager::get_wallpaper_count()) 
			current_index = 0;
		return WallpaperManager::get_wallpaper(current_index);
	}
	std::string get_previous_wallpaper() {
		current_index--;
		if (current_index < 0)
			current_index += WallpaperManager::get_wallpaper_count();
		return WallpaperManager::get_wallpaper(current_index);
	}

}