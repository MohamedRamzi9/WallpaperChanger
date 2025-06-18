#include "WallpaperChangerService.hpp"
#include "WallpaperChanger.hpp"
#include "WallpaperManager.hpp"
#include "App.hpp"

#include "rmz_print.hpp"

#include <thread>
#include <atomic>

namespace WallpaperChangerService {

	enum State { RUNNING, PAUSED, STOPPED };
	std::atomic<State> state;
	std::counting_semaphore<0> pause_semaphore(0);
	std::counting_semaphore<0> empty_semaphore(0);
	std::thread thread;

	void initialize() {}
	void start() {
		thread = std::thread([] {
			while (true) {
				if (state.load() == STOPPED) {
					return;
				} else if (state.load() == PAUSED) {
					pause_semaphore.acquire();
					continue;
				}
				if (WallpaperManager::empty()) {
					empty_semaphore.acquire();
					continue;
				} else if (not WallpaperChanger::is_timer_done()) {
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
					continue;
				}
				WallpaperChanger::init_timer();
				std::string wallpaper = WallpaperChanger::set_next_wallpaper();
				App::set_wallpaper_changed_message(wallpaper);
				App::signal();
			}
		});
	}
	void stop() {
		state.store(STOPPED);
		empty_semaphore.release(); 
		pause_semaphore.release();
		thread.join();
	}
	void pause() { state.store(PAUSED); }
	void resume() { state.store(RUNNING); pause_semaphore.release(); }
	void notify_added_wallpaper() { empty_semaphore.release(); }

	bool is_running() { return state.load() == RUNNING; }
	bool is_stopped() { return state.load() == STOPPED; }
	
}