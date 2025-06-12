

#include "App.hpp"
#include "CommandManager.hpp"
#include "WallpaperManager.hpp"
#include "Utility.hpp"
#include "Global.hpp"

#include <ranges>
#include <conio.h>
#include <ranges>

#include "rmz_console.hpp"
#include "rmz_print.hpp"







namespace App {
    menu_type menu;

    std::string error_message;
    std::string info_message;
    std::string wallpaper_changed_message;
    bool show_parameters_flag = false;
    std::atomic<bool> signal_flag = false;
    std::atomic<bool> is_getting_input_flag = false;

    enum input_enum { STRING, KEY } input_type = KEY;
    input_key_type input_key;
    std::string input_string;

    void initialize() { menu = MAIN_MENU; }
    void set_error_message(const std::string& message) { error_message = message; }
    void set_info_message(const std::string& message) { info_message = message; }
    void show_parameters() { show_parameters_flag = true; }
    void set_wallpaper_changed_message(const std::string& message) { wallpaper_changed_message = message; }
    void signal() { 
        signal_flag.store(true);
        if (is_getting_input_flag.load())
            rmz::insert_enter();
    }

    void clear_error_message() { error_message.clear(); }
    void clear_info_message() { info_message.clear(); }
    void clear_parameters() { show_parameters_flag = false; }
    void clear_wallpaper_changed_message() { wallpaper_changed_message.clear(); }

    bool update_input_key() {
        if (_kbhit()) {
            char c = getch();
            if (c == 13) input_key = input_key_type({ENTER, c});
            else if (c == 0 or c == 224) {
                c = getch(); // Handle special keys
                if (c == 72) input_key = input_key_type({UP, c});    // Up arrow key
                if (c == 80) input_key = input_key_type({DOWN, c});  // Down arrow key
                if (c == 75) input_key = input_key_type({LEFT, c});  // Left arrow key
                if (c == 77) input_key = input_key_type({RIGHT, c}); // Right arrow key
            } else {
                input_key = input_key_type({CHARACTER, c});
            }
            return true;
        }
        return false;
    }
    input_key_type& get_input_key() { return input_key; }
    rmz::type::nullable<std::string> update_input_string() {
        rmz::type::nullable<std::string> result{};
        std::string input;
        is_getting_input_flag.store(true);
        std::getline(std::cin, input);
        is_getting_input_flag.store(false);
        if (signal_flag.load()) {
            result = {input};
        } else {
            App::input_string = input;
        }

        return result;
    }
    std::string& get_input_string() { return input_string; }

    void clear_console() { rmz::clear_console(); } 
    void set_menu(menu_type new_menu) { 
        menu = new_menu; 
        clear_info_message();    
    }


    void render() {
        rmz::clear_console();
        if (not wallpaper_changed_message.empty())
            rmz::println("* Wallpaper Changed: {}\n", wallpaper_changed_message);
        if (not error_message.empty()) 
            rmz::println("* Error: {}\n", error_message);
        if (show_parameters_flag) {
            rmz::println("* Parameters:\n");
            rmz::println(" - Save File: {}", save_file);
            rmz::println(" - Auto Save: {}", auto_save);
            rmz::println(" - Wallpaper Order: {}", WallpaperChanger::is_change_order_order() ? "Sequential" : "Random");
            rmz::println(" - Wallpaper Duration: {}s", WallpaperChanger::get_duration().count());
            rmz::println();
        }
        if (not info_message.empty())
            rmz::println("* Info: {}\n", info_message);

        if (menu == MAIN_MENU) MainMenu::render();
        else if (menu == DURATION_MENU) DurationMenu::render();
    }
    void update() {
        auto [type, c] = get_input_key();
        rmz::println("Input Type: {}", int(type));
        if (type == LEFT) WallpaperChanger::set_next_wallpaper();
        else if (type == RIGHT) WallpaperChanger::set_previous_wallpaper();
        else if (menu == MAIN_MENU) MainMenu::update();
        else if (menu == DURATION_MENU) DurationMenu::update();
    }
    void run() {
        rmz::enable_ansi();

        render();
        while (true) {
            if (signal_flag.load()) {
                signal_flag.store(false);
                render();
                continue;
            }
            if (input_type == KEY) {
                if (not update_input_key()) {
                    continue;
                }
            } else if (input_type == STRING) {
                if (auto result = update_input_string()) { 
                    signal_flag.store(false);
                    render();
                    rmz::insert_input(result.get());
                    continue;
                }
            }
            update();
            if (state.load() == STOPPED) break;
            render();
        }
    }


    namespace MainMenu { 
        int choice;

        void initialize() { choice = 0; }
        void update() {
            auto [type, c] = get_input_key();
            if (type == UP and choice > 0) choice--;
            else if (type == DOWN and choice < CommandManager::get_command_count() - 1) choice++;
            else if (type == CHARACTER) {
                if (c == 'e') {
                    rmz::print("Exiting...\n");
                    state.store(STOPPED);
                } else if (c == 'a') {
                    App::clear_console();
                    App::clear_error_message();
                    rmz::println("Enter the folder path to wallpapers: ");
                    auto folder = App::get_input_string();
                    CommandManager::Add::run(folder);
                    App::set_info_message(rmz::format("Added wallpaper: {}", folder));

                } else if (c == 'o') {
                    CommandManager::Order::run();

                } else if (c == 'r') {
                    CommandManager::Random::run();
                
                } else if (c == 'd') {
                    App::set_menu(App::DURATION_MENU);

                } else if (c == 'p') {
                    CommandManager::Pause::run();
                    App::set_info_message("Paused wallpaper changer");

                } else if (c == 'c') {
                    CommandManager::Resume::run();
                    App::set_info_message("Resumed wallpaper changer");

                } else if (c == 'i') {
                    CommandManager::Parameters::run();

                } else if (c == 'x') {
                    App::clear_info_message();
                    App::clear_error_message();
                    App::clear_parameters();
                }
            }
        }
        void render() {
            rmz::println("* Wallpaper Changer Main Menu\n");
            rmz::println(" - 'e' - Exit");
            rmz::println(" - 'a' - Add Wallpaper Folder");
            rmz::println(" - 'o' - Set Order of Wallpapers to Sequential");
            rmz::println(" - 'r' - Set Order of Wallpapers to Random");
            rmz::println(" - 'd' - Set Duration for Wallpaper Change");
            rmz::println(" - 'p' - Pause Wallpaper Changer");
            rmz::println(" - 'c' - Resume Wallpaper Changer");
            rmz::println(" - 'i' - Show Parameters");
            rmz::println(" - 'x' - Clear Info and Error Messages");
        }
    }

    namespace DurationMenu {
        bool seconds;
        bool is_getting_duration = false;

        void initialize() {}
        void update() {
            if (not is_getting_duration) { 
                auto [type, c] = App::get_input_key();
                if (type == CHARACTER) {
                    if (c == 's') {
                        seconds = true;
                        is_getting_duration = true;
                    } else if (c == 'm') {
                        seconds = false;
                        is_getting_duration = true;
                    } 
                    if (is_getting_duration) 
                        input_type = STRING;
                }
                return;
            } else {
                auto input_duration = App::get_input_string();
                int value = stoi(input_duration);
                if (seconds) 
                    CommandManager::Duration::run(rmz::seconds(value));
                else
                    CommandManager::Duration::run(rmz::minutes(value));
                App::set_info_message(rmz::format("Set duration to {} {}", value, seconds ? "seconds" : "minutes"));
                App::set_menu(App::MAIN_MENU);
                is_getting_duration = false;
                input_type = KEY;
            }
        }
        void render() {
            if (not is_getting_duration) {
                rmz::println("Duration Menu");
                rmz::println("Press 's' for seconds or 'm' for minutes");
            } else {
                rmz::print("Enter the duration: ");
            }
            
        }
    }


}

