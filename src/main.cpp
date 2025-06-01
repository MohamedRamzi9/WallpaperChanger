
#include <thread>

#include "ConsoleManager.hpp"
#include "WallpaperChanger.hpp"
#include "WallpaperManager.hpp"
#include "WallpaperGetter.hpp"
#include "CommandManager.hpp"
#include "Parameter.hpp"
#include "Other.hpp"

#include <filesystem>

#include "rmz_print.hpp"

std::counting_semaphore<0> pause_semaphore(0);
std::counting_semaphore<0> empty_semaphore(0);
std::atomic<State> state{RUNNING};
std::string save_file = "settings.wallpaper";
bool auto_save = true;


bool action(const std::string& input) {
    bool valid_command = true;
    
    if (CommandManager::Order::parse(input)) {
        WallpaperChanger::set_change_order(WallpaperChanger::ORDER);

    } else if (CommandManager::Random::parse(input)) {
        WallpaperChanger::set_change_order(WallpaperChanger::RANDOM);
        
    } else if (auto result = CommandManager::Duration::parse(input)) {
        WallpaperChanger::set_duration(result.get());

    } else if (CommandManager::Pause::parse(input)) {
        state.store(PAUSED);

    } else if (auto result = CommandManager::Add::parse(input)) {
        WallpaperManager::add_folder(result.get());
        WallpaperRandomGetter::refresh();
        WallpaperOrderGetter::refresh();
        empty_semaphore.release();

    } else if (CommandManager::Resume::parse(input)) {
        state.store(RUNNING);
        pause_semaphore.release();

    } else if (CommandManager::Next::parse(input)) {
        WallpaperChanger::set_next_wallpaper();

    } else if (CommandManager::Previous::parse(input)) {
        WallpaperChanger::set_previous_wallpaper();

    } else if (auto result = CommandManager::Set::parse(input)) {
        WallpaperChanger::set_wallpaper(result.get());
        ConsoleManager::set_wallpaper_changed_message(std::format("* Wallpaper set to: '{}'", result.get()));
        ConsoleManager::signal_update();

    } else if (CommandManager::Help::parse(input)) {
        ConsoleManager::show_help();

    } else if (CommandManager::Parameters::parse(input)) {
        ConsoleManager::show_parameters();

    // } else if (auto result = CommandManager::SaveFile::parse(input)) {
    //     save_file = result.get();

    } else if (CommandManager::Save::parse(input)) {
        Parameters::save(save_file);
        
    } else if (auto result = CommandManager::AutoSave::parse(input)) {
        auto_save = result.get();
    
    // } else if (auto result = CommandManager::Load::parse(input)) {
    //     Parameters::load(result.get());

    } else if (auto result = CommandManager::Clear::parse(input)) {
        auto parameter = result.get();
        if (CommandManager::Clear::is_clear_all(parameter)) {
            ConsoleManager::clear_help();
            ConsoleManager::clear_parameters();
            ConsoleManager::clear_error_message();
        } else if (CommandManager::Clear::is_clear_help(parameter))
            ConsoleManager::clear_help();
        else if (CommandManager::Clear::is_clear_parameters(parameter))
            ConsoleManager::clear_parameters();
        else if (CommandManager::Clear::is_clear_error(parameter))
            ConsoleManager::clear_error_message();
    
    } else {
        valid_command = false;
    }

    if (valid_command) {
        ConsoleManager::clear_error_message();
    }
    
    return valid_command;
}




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

        ConsoleManager::set_wallpaper_changed_message(std::format("* New wallpaper : '{}'", wallpaper));
        ConsoleManager::signal_update();

    }
}




// ========================================
// ============ MAIN FUNCTION =============
// ========================================

void main_function() {

    // Path to the image file
    ConsoleManager::initialize();
    WallpaperManager::initialize(); 
    WallpaperRandomGetter::initialize();
    WallpaperOrderGetter::initialize();
    WallpaperChanger::initialize();
    Parameters::initialize();
    CommandManager::initialize();

    // Load parameters from file
    if (std::filesystem::exists(save_file))
        Parameters::load(save_file);

    std::thread wallpaper_thread(service_wallpaper_changer);


    while (true) {
        ConsoleManager::update();
        auto input = ConsoleManager::get_input();

        if (input.empty()) {
            continue;
        }

        if (CommandManager::Exit::parse(input)) {
            state.store(STOPPED);
            pause_semaphore.release(); 
            empty_semaphore.release(); 
            break;

        } else {
            if (not action(input)) {
                ConsoleManager::set_error_message(std::format("* Command '{}' not recognized", input));
            }
        }
            
    }

    if (auto_save)
        Parameters::save("settings.wallpaper");

    wallpaper_thread.join();
}


void test_function() {
}

int main() {

    main_function();
    // test_function();
    return 0;
}
