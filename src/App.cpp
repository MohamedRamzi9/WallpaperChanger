

#include "App.hpp"
#include "CommandManager.hpp"
#include "WallpaperManager.hpp"
#include "Parameter.hpp"
#include "Utility.hpp"
#include "Global.hpp"

#include <ranges>
#include <conio.h>
#include <ranges>
#include <filesystem>

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
            else if (c == 27) input_key = input_key_type({ESCAPE, c}); // Escape key
            else if (c == 0 or c == 224) {
                c = getch(); // Handle special keys
                if (c == 72) input_key = input_key_type({UP, c});    // Up arrow key
                else if (c == 80) input_key = input_key_type({DOWN, c});  // Down arrow key
                else if (c == 75) input_key = input_key_type({LEFT, c});  // Left arrow key
                else if (c == 77) input_key = input_key_type({RIGHT, c}); // Right arrow key
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
    void set_input_type(input_enum type) { input_type = type; }
    bool is_input_string() { return input_type == STRING; }
    bool is_input_key() { return input_type == KEY; }
        
    void clear_console() { rmz::clear_console(); } 
    void set_menu(menu_type new_menu) { 
        menu = new_menu; 
        if (new_menu == MAIN_MENU or new_menu == DURATION_MENU or new_menu == REMOVE_MENU) {
            set_input_type(KEY);
        } else if (new_menu == ADD_MENU) {
            set_input_type(STRING);
        } 
    }


    void render() {
        rmz::clear_console();
        if (not wallpaper_changed_message.empty())
            rmz::println("* Wallpaper Changed: {}\n", wallpaper_changed_message);
        if (not info_message.empty())
            rmz::println("* Info: {}\n", info_message);
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

        if (menu == MAIN_MENU) MainMenu::render();
        else if (menu == DURATION_MENU) DurationMenu::render();
        else if (menu == ADD_MENU) AddMenu::render();
        else if (menu == REMOVE_MENU) RemoveMenu::render();
        else rmz::println("* Unknown Menu\n");
    }
    void update() {
        auto [type, c] = get_input_key();
        if (type == LEFT and WallpaperManager::get_wallpaper_count() > 0) WallpaperChanger::set_next_wallpaper();
        else if (type == RIGHT and WallpaperManager::get_wallpaper_count() > 0) WallpaperChanger::set_previous_wallpaper();
        else if (menu == MAIN_MENU) MainMenu::update();
        else if (menu == ADD_MENU) AddMenu::update();
        else if (menu == DURATION_MENU) DurationMenu::update();
        else if (menu == REMOVE_MENU) RemoveMenu::update();
    }
    void run() {
        rmz::enable_ansi();

        auto treat_signal = [] {
            signal_flag.store(false);
            render();
        };

        render();
        while (true) {
            if (input_type == KEY) {
                while (not update_input_key()) {
                    if (signal_flag.load()) {
                        treat_signal();
                    }
                }
            } else if (is_input_string()) {
                while (auto result = update_input_string()) {
                    treat_signal();
                    rmz::insert_input(result.get());
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
                    empty_semaphore.release(); 
                    pause_semaphore.release();

                } else if (c == 'a') {
                    App::set_menu(App::ADD_MENU);

                } else if (c == 't') {
                    if (WallpaperManager::get_wallpaper_count() == 0) {
                        App::set_error_message("No wallpaper folders to remove.");
                    } else {
                        App::set_menu(App::REMOVE_MENU);
                        App::clear_error_message();
                        App::clear_info_message();
                    }

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

                } else if (c == 'u') {
                    WallpaperManager::load_all_wallpapers();
                    WallpaperChanger::refresh();
                    if (WallpaperManager::get_wallpaper_count() > 0) {
                        empty_semaphore.release();
                    }
                } else if (c == 'f') {
                    std::string folders_message = "Wallpaper Folders:\n";
                    for (const auto& folder : WallpaperManager::get_folders()) {
                        folders_message += folder + "\n";
                    }
                    App::set_info_message(folders_message);
                    
                } else if (c == 's') {
                    Parameters::save(save_file);
                    App::set_info_message("Parameters saved to " + save_file);
                }
            } 
        }
        void render() {
            rmz::println("* Wallpaper Changer Main Menu\n");
            rmz::println(" - 'e' - Exit");
            rmz::println(" - 'a' - Add Wallpaper Folder");
            rmz::println(" - 't' - Remove Wallpaper Folder");
            rmz::println(" - 'o' - Set Order of Wallpapers to Sequential");
            rmz::println(" - 'r' - Set Order of Wallpapers to Random");
            rmz::println(" - 'd' - Set Duration for Wallpaper Change");
            rmz::println(" - 'p' - Pause Wallpaper Changer");
            rmz::println(" - 'c' - Resume Wallpaper Changer");
            rmz::println(" - 'i' - Show Parameters");
            rmz::println(" - 'x' - Clear Info and Error Messages");
            rmz::println(" - 'u' - Reload all Wallpapers");
            rmz::println(" - 'f' - Show all Wallpaper Folders");
            rmz::println(" - 's' - Save Parameters");
        }
    }

    namespace DurationMenu {
        bool seconds;
        bool is_getting_duration = false;

        void initialize() {}
        void update() {
            if (not is_getting_duration) { 
                auto [type, c] = App::get_input_key();
                if (type == ESCAPE) {
                    App::set_menu(App::MAIN_MENU);
                } else if (type == CHARACTER) {
                    if (c == 's') {
                        seconds = true;
                        is_getting_duration = true;
                    } else if (c == 'm') {
                        seconds = false;
                        is_getting_duration = true;
                    } 
                    if (is_getting_duration) 
                        App::set_input_type(App::STRING);
                }
                return;
            } else {
                auto input_duration = App::get_input_string();
                if (not input_duration.empty()) {
                    int value = stoi(input_duration);
                    if (seconds) 
                        CommandManager::Duration::run(rmz::seconds(value));
                    else
                        CommandManager::Duration::run(rmz::minutes(value));
                    App::set_info_message(rmz::format("Set duration to {} {}", value, seconds ? "seconds" : "minutes"));
                }
                is_getting_duration = false;
                App::set_input_type(App::KEY);
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

    namespace AddMenu {
        void initialize() {}
        void update() {
            auto folder = App::get_input_string();
            if (not folder.empty()) {
                if (std::filesystem::exists(folder) && std::filesystem::is_directory(folder)) {
                    CommandManager::Add::run(folder);
                    auto [wallpapers, count] = count_wallpapers_message(folder);
                    App::set_info_message(rmz::format("Added folder: '{}' containing {} wallpapers:\n{}", folder, count, wallpapers));
                } else {
                    App::set_error_message(rmz::format("Invalid folder: '{}'", folder));
                }
            }
            App::set_menu(App::MAIN_MENU);
        }
        void render() {
            rmz::println("* Add Wallpaper Folder Menu\n");
            rmz::print("Enter the folder path that contains the wallpapers: ");
        }
    }

    namespace RemoveMenu {
        int choice = 0;
        void initialize() {}
        void update() {
            auto [type, c] = get_input_key();
            if (type == UP and choice > 0) choice--;
            else if (type == DOWN and choice < WallpaperManager::get_folders().size() - 1) choice++;
            else if (type == ESCAPE) App::set_menu(App::MAIN_MENU);
            else if (type == ENTER) {
                auto& folder = WallpaperManager::get_folders()[choice];
                CommandManager::Remove::run(folder);
                auto [wallpapers, count] = count_wallpapers_message(folder);
                App::set_info_message(rmz::format("Removed folder: '{}' containing {} wallpapers:\n{}", folder, count, wallpapers));
                App::set_menu(App::MAIN_MENU);
            }
        }
        void render() {
            for (int i = 0; i < WallpaperManager::get_folders().size(); i++) {
                if (i == choice) {
                    rmz::print("-> ");
                }
                rmz::println(WallpaperManager::get_folders()[i]);
            }
        }
    }







}