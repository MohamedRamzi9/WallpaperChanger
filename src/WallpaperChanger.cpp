#include "WallpaperChanger.hpp"
#include "WallpaperGetter.hpp"

#include <windows.h>

enum WallpaperChanger::change_order WallpaperChanger::change_order;
rmz::timer WallpaperChanger::timer; 

void WallpaperChanger::initialize() {
	change_order = ORDER;
	timer.set_duration(duration_type(10)); 
	timer.init(); 
}
void WallpaperChanger::set_change_order(enum change_order new_order) {
	change_order = new_order;
}
void WallpaperChanger::set_wallpaper(const std::string& path) {
	SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (PVOID)path.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
}
std::string WallpaperChanger::set_next_wallpaper() {
	std::string wallpaper;
	if (change_order == ORDER) {
		wallpaper = WallpaperOrderGetter::get_next_wallpaper();
	} else {
		wallpaper = WallpaperRandomGetter::get_next_wallpaper();
	}
	set_wallpaper(wallpaper);
	return wallpaper;
}
std::string WallpaperChanger::set_previous_wallpaper() {
	std::string wallpaper;
	if (change_order == ORDER) {
		wallpaper = WallpaperOrderGetter::get_previous_wallpaper();
	} else {
		wallpaper = WallpaperRandomGetter::get_previous_wallpaper();
	}
	set_wallpaper(wallpaper);
	return wallpaper;
}
bool WallpaperChanger::is_change_order_random() {
	return change_order == RANDOM;
}
bool WallpaperChanger::is_change_order_order() {
	return change_order == ORDER;
}
WallpaperChanger::duration_type WallpaperChanger::get_duration() {
	return timer.get_duration();
}
bool WallpaperChanger::is_timer_done() {
	return timer.is_done();
}
void WallpaperChanger::set_duration(duration_type duration) {
	timer.set_duration(duration);
	timer.init();
}
void WallpaperChanger::init_timer() {
	timer.init();
}
