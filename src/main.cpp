
#include <thread>

#include "ConsoleManager.hpp"
#include "WallpaperChanger.hpp"
#include "WallpaperManager.hpp"
#include "WallpaperGetter.hpp"
#include "CommandManager.hpp"
#include "Parameter.hpp"
#include "Utility.hpp"
#include "Other.hpp"

#include <filesystem>
#include <conio.h>
#include <ranges>

#include "rmz_console.hpp"
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
    
    } else if (auto result = CommandManager::Load::parse(input)) {
        Parameters::load(result.get());

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










enum input_enum { UP, DOWN, LEFT, RIGHT, CHARCTER, ENTER };
using input_type = std::pair<input_enum,char>;
rmz::type::nullable<input_type> get_input() {
    if (_kbhit()) {
        char c = getch();
        if (c == 13) return {{ENTER, c}};
        if (c == 0 or c == 224) {
            c = getch(); // Handle special keys
            if (c == 72) return {{UP, c}};    // Up arrow key
            if (c == 80) return {{DOWN, c}};  // Down arrow key
            if (c == 75) return {{LEFT, c}};  // Left arrow key
            if (c == 77) return {{RIGHT, c}}; // Right arrow key
        }
       return {{CHARCTER, c}};
    }
    return {};
}


namespace App {
    enum menu_type { MAIN_MENU, } menu;
    void initialize() { menu = MAIN_MENU; }
    namespace MainMenu {
        int choice;
        void initialize() { choice = 0; }
        void update(input_type input) {
            auto [type, c] = input;
            if (type == UP and choice > 0) choice--;
            else if (type == DOWN and choice < CommandManager::get_command_count() - 1) choice++;
            else if (type == ENTER) {
                auto& command = CommandManager::get_command(choice);
                
            }
        }
        void render() {
            for (auto [i, command] : std::ranges::views::enumerate(CommandManager::get_commands())) {
                if (choice == i) rmz::print("-> ");
                rmz::println(command.string);
            }
        }
    }

    void render() {
        rmz::clear_console();
        if (menu == MAIN_MENU) MainMenu::render();
    }
    void update(auto input) {
        auto [type, c] = input;
        if (type == LEFT) WallpaperChanger::set_next_wallpaper();
        else if (type == RIGHT) WallpaperChanger::set_previous_wallpaper();
        else if (menu == MAIN_MENU) MainMenu::update(input);
    }
    void run() {
        rmz::enable_ansi();

        render();
        while (true) {
            if (auto input = get_input()) {
                update(input.get());
                render();
            }
        }
    }
}



// ========================================
// ============ MAIN FUNCTION =============
// ========================================

void main_function() {

    // Path to the image file
    // ConsoleManager::initialize();
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

    // std::thread wallpaper_thread(service_wallpaper_changer);

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
