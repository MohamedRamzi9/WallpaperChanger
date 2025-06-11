
#include <thread>

#include "App.hpp"
#include "WallpaperChanger.hpp"
#include "WallpaperManager.hpp"
#include "WallpaperGetter.hpp"
#include "CommandManager.hpp"
#include "Parameter.hpp"
#include "Utility.hpp"
#include "Global.hpp"

#include <filesystem>






// =================================================
// ========== WALLPAPER CHANGER SERVICE ============
// =================================================


void service_wallpaper_changer() {
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
}










// ========================================
// ============ MAIN FUNCTION =============
// ========================================

void main_function() {

    // Path to the image file
    WallpaperManager::initialize(); 
    WallpaperRandomGetter::initialize();
    WallpaperOrderGetter::initialize();
    WallpaperChanger::initialize();
    Parameters::initialize();
    CommandManager::initialize();
    App::initialize();

    // Load parameters from file
    if (std::filesystem::exists(save_file))
        Parameters::load(save_file);

    std::thread wallpaper_thread(service_wallpaper_changer);

    App::run();

    if (auto_save)
        Parameters::save("settings.wallpaper");

    // wallpaper_thread.join();
}

void test_function() {
}

int main() {

    main_function();
    // test_function();
    return 0;
}
