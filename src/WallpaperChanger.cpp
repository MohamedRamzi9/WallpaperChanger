#include "WallpaperChanger.hpp"
#include "WallpaperGetter.hpp"

#include <windows.h>

namespace WallpaperChanger {

	enum change_order change_order;
	rmz::timer timer; 

	void initialize() {
		change_order = ORDER;
		timer.set_duration(duration_type(10)); 
		timer.init(); 
	}
	void set_change_order(enum change_order new_order) {
		change_order = new_order;
	}
	void set_wallpaper(const std::string& path) {
		SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (PVOID)path.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
	}
	std::string set_next_wallpaper() {
		std::string wallpaper;
		if (change_order == ORDER) {
			wallpaper = WallpaperOrderGetter::get_next_wallpaper();
		} else {
			wallpaper = WallpaperRandomGetter::get_next_wallpaper();
		}
		set_wallpaper(wallpaper);
		return wallpaper;
	}
	std::string set_previous_wallpaper() {
		std::string wallpaper;
		if (change_order == ORDER) {
			wallpaper = WallpaperOrderGetter::get_previous_wallpaper();
		} else {
			wallpaper = WallpaperRandomGetter::get_previous_wallpaper();
		}
		set_wallpaper(wallpaper);
		return wallpaper;
	}
	bool is_change_order_random() {
		return change_order == RANDOM;
	}
	bool is_change_order_order() {
		return change_order == ORDER;
	}
	duration_type get_duration() {
		return timer.get_duration(); 
	}
	bool is_timer_done() {
		return timer.is_done();
	}
	void set_duration(duration_type duration) {
		timer.set_duration(duration);
		timer.init();
	}
	void init_timer() {
		timer.init();
	}

}