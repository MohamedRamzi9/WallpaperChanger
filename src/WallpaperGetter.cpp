
#include "WallpaperGetter.hpp"
#include "WallpaperManager.hpp"

#include <algorithm>


int WallpaperRandomGetter::current_index;
std::vector<int> WallpaperRandomGetter::indices;
std::mt19937 WallpaperRandomGetter::rng;

void WallpaperRandomGetter::initialize() {
	rng = std::mt19937(std::random_device()());
	
	current_index = 0;
	indices.resize(WallpaperManager::get_wallpaper_count());
	std::iota(indices.begin(), indices.end(), 0);
	std::shuffle(indices.begin(), indices.end(), rng);
}
void WallpaperRandomGetter::refresh() {
	current_index = 0;
	indices.resize(WallpaperManager::get_wallpaper_count());
	std::iota(indices.begin(), indices.end(), 0);
	std::shuffle(indices.begin(), indices.end(), rng);
}
void WallpaperRandomGetter::reset() {
	current_index = 0;
}
std::string WallpaperRandomGetter::get_next_wallpaper() {
	current_index++;
	if (current_index >= indices.size()) 
		current_index = 0;
	return WallpaperManager::get_wallpaper(indices[current_index]);
}
std::string WallpaperRandomGetter::get_previous_wallpaper() {
	current_index--;
	if (current_index < 0) 
		current_index += indices.size();
	return WallpaperManager::get_wallpaper(indices[current_index]);
}




int WallpaperOrderGetter::current_index;

void WallpaperOrderGetter::initialize() {
	current_index = 0;
}
void WallpaperOrderGetter::refresh() {}
void WallpaperOrderGetter::reset() {
	current_index = 0;
}
std::string WallpaperOrderGetter::get_next_wallpaper() {
	current_index++;
	if (current_index >= WallpaperManager::get_wallpaper_count()) 
		current_index = 0;
	return WallpaperManager::get_wallpaper(current_index);
}
std::string WallpaperOrderGetter::get_previous_wallpaper() {
	current_index--;
	if (current_index < 0)
		current_index += WallpaperManager::get_wallpaper_count();
	return WallpaperManager::get_wallpaper(current_index);
}

