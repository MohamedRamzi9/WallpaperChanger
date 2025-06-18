
#pragma once

namespace WallpaperChangerService {

	void initialize();
	void start();
	void stop();
	void pause();
	void resume();
	void notify_added_wallpaper();

	bool is_running();
	bool is_stopped();
}