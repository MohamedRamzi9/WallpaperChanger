

#include "App.hpp"
#include "CommandManager.hpp"
#include "WallpaperManager.hpp"
#include "Parameter.hpp"
#include "Utility.hpp"
#include "Global.hpp"
#include "WallpaperChangerService.hpp"

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
    
    void initialize() { 
        set_menu(MAIN_MENU);
        Input::initialize(); 
        Input::set_input_key();
    }
    void set_error_message(const std::string& message) { error_message = message; }
    void set_info_message(const std::string& message) { info_message = message; }
    void show_parameters() { show_parameters_flag = true; }
    void set_wallpaper_changed_message(const std::string& message) { wallpaper_changed_message = message; }
    
    void clear_error_message() { error_message.clear(); }
    void clear_info_message() { info_message.clear(); }
    void clear_parameters() { show_parameters_flag = false; }
    void clear_wallpaper_changed_message() { wallpaper_changed_message.clear(); }

    void signal() { 
        signal_flag.store(true);
        if (Input::is_getting_input())
            Input::stop_getting_input();
    }

    namespace Input {
        HANDLE hStdin;
        input_enum input_type;
        INPUT_RECORD input_key;
        std::string input_string;
        std::atomic<bool> is_getting_input_flag(false);

        void initialize() {
            hStdin = GetStdHandle(STD_INPUT_HANDLE);
            if (hStdin == INVALID_HANDLE_VALUE) {
                rmz::println("Error: Unable to get standard input handle.");
                std::exit(1); 
            }
        }
        void set_input_key() {
            SetConsoleMode(hStdin, ENABLE_WINDOW_INPUT | ENABLE_PROCESSED_INPUT); 
            input_type = KEY;
        }
        void set_input_string() { 
            SetConsoleMode(hStdin, ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT); 
            FlushConsoleInputBuffer(hStdin); // Clear any unread key events
            input_type = STRING;
        }

        bool is_getting_input() { return is_getting_input_flag.load(); }
        bool is_input_string() { return input_type == STRING; }
        bool is_input_key() { return input_type == KEY; }

        void stop_getting_input() { rmz::insert_enter(); }
        void insert_input(const std::string& input) { rmz::insert_input(input); }

        void update_input_key() {
            constexpr std::array valid_keys = { VK_ESCAPE, VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, VK_RETURN };
            auto is_valid_key = [valid_keys](INPUT_RECORD& keyEvent) {
                return std::ranges::find(valid_keys, keyEvent.Event.KeyEvent.wVirtualKeyCode) != valid_keys.end()
                    or (keyEvent.Event.KeyEvent.uChar.AsciiChar >= 32 and keyEvent.Event.KeyEvent.uChar.AsciiChar <= 126);
            };

            INPUT_RECORD inputRecord;
            DWORD events;
            while (true) {
                is_getting_input_flag.store(true);
                ReadConsoleInput(hStdin, &inputRecord, 1, &events);
                is_getting_input_flag.store(false);
                if (inputRecord.Event.KeyEvent.bKeyDown and is_valid_key(inputRecord)) {
                    input_key = inputRecord;
                    break;
                }
            }

        }
        void update_input_string() {
            is_getting_input_flag.store(true);
            std::getline(std::cin, input_string);
            is_getting_input_flag.store(false);
        }
        std::string& get_input_string() { return input_string; }
        char get_char() { return input_key.Event.KeyEvent.uChar.AsciiChar; }
        bool is_key_escape() { return input_key.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE; }
        bool is_key_up() { return input_key.Event.KeyEvent.wVirtualKeyCode == VK_UP; }
        bool is_key_down() { return input_key.Event.KeyEvent.wVirtualKeyCode == VK_DOWN; }
        bool is_key_left() { return input_key.Event.KeyEvent.wVirtualKeyCode == VK_LEFT; }
        bool is_key_right() { return input_key.Event.KeyEvent.wVirtualKeyCode == VK_RIGHT; }
        bool is_key_enter() { return input_key.Event.KeyEvent.wVirtualKeyCode == VK_RETURN; }
        bool is_key_char() { return input_key.Event.KeyEvent.uChar.AsciiChar >= 32 and input_key.Event.KeyEvent.uChar.AsciiChar <= 126; }
    }
    
    
    void clear_console() { rmz::clear_console(); } 
    void set_menu(menu_type new_menu) { 
        menu = new_menu; 
        if (new_menu == MAIN_MENU or new_menu == DURATION_MENU or new_menu == REMOVE_MENU) {
            Input::set_input_key();
        } else if (new_menu == ADD_MENU) {
            Input::set_input_string();
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
            rmz::println(" - Wallpaper Order: {}", WallpaperChanger::is_change_order_sequential() ? "Sequential" : "Random");
            rmz::println(" - Wallpaper Duration: {}s", WallpaperChanger::get_duration().count());
            rmz::println(" - Wallpaper Changer: {}", WallpaperChangerService::is_running() ? "Running" : "Paused");
            rmz::println();
        }

        if (menu == MAIN_MENU) MainMenu::render();
        else if (menu == DURATION_MENU) DurationMenu::render();
        else if (menu == ADD_MENU) AddMenu::render();
        else if (menu == REMOVE_MENU) RemoveMenu::render();
        else rmz::println("* Unknown Menu\n");
    }
    void update() {
        // auto [type, c] = Input::get_input_key();
        if (Input::is_key_left() and WallpaperManager::get_wallpaper_count() > 0) 
            set_wallpaper_changed_message(WallpaperChanger::set_next_wallpaper());
        else if (Input::is_key_right() and WallpaperManager::get_wallpaper_count() > 0)
            set_wallpaper_changed_message(WallpaperChanger::set_previous_wallpaper());
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
            if (Input::is_input_key()) {
                Input::update_input_key();
                if (signal_flag.load()) {
                    treat_signal();
                    continue; 
                }
            } else if (Input::is_input_string()) {
                Input::update_input_string();
                if (signal_flag.load()) {
                    treat_signal();
                    Input::insert_input(Input::get_input_string());
                    continue;
                }
            }
            update();
            if (WallpaperChangerService::is_stopped()) break;
            render();
        }
    }


    namespace MainMenu { 
        int choice;

        void initialize() { choice = 0; }
        void update() {
            // auto [type, c] = Input::get_input_key();
            if (Input::is_key_up() and choice > 0) choice--;
            else if (Input::is_key_down() and choice < CommandManager::get_command_count() - 1) choice++;
            else if (Input::is_key_char()) {
                char c = Input::get_char();
                if (c == 'e') {
                    rmz::print("Exiting...\n");
                    WallpaperChangerService::stop();

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
                    WallpaperChanger::set_change_order_sequential();

                } else if (c == 'r') {
                    WallpaperChanger::set_change_order_random();
                
                } else if (c == 'd') {
                    App::set_menu(App::DURATION_MENU);

                } else if (c == 'p') {
                    pause_wallpaper_changer();
                    App::set_info_message("Paused wallpaper changer");

                } else if (c == 'c') {
                    resume_wallpaper_changer();
                    App::set_info_message("Resumed wallpaper changer");

                } else if (c == 'i') {
                    App::show_parameters();

                } else if (c == 'x') {
                    App::clear_info_message();
                    App::clear_error_message();
                    App::clear_parameters();

                } else if (c == 'u') {
                    WallpaperManager::load_all_wallpapers();
                    WallpaperChanger::refresh();
                    if (WallpaperManager::get_wallpaper_count() > 0) {
                        WallpaperChangerService::notify_added_wallpaper();
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
                // auto [type, c] = Input::get_input_key();
                if (Input::is_key_escape()) {
                    App::set_menu(App::MAIN_MENU);
                } else if (Input::is_key_char()) {
                    char c = Input::get_char();
                    if (c == 's') {
                        seconds = true;
                        is_getting_duration = true;
                    } else if (c == 'm') {
                        seconds = false;
                        is_getting_duration = true;
                    } 
                    if (is_getting_duration) 
                        Input::set_input_string();
                }
                return;
            } else {
                auto input_duration = Input::get_input_string();
                if (not input_duration.empty()) {
                    int value = stoi(input_duration);
                    if (seconds) 
                        WallpaperChanger::set_duration(rmz::seconds(value));
                    else
                        WallpaperChanger::set_duration(rmz::minutes(value));
                    App::set_info_message(rmz::format("Set duration to {} {}", value, seconds ? "seconds" : "minutes"));
                }
                is_getting_duration = false;
                Input::set_input_key();
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
            auto folder = Input::get_input_string();
            if (not folder.empty()) {
                if (std::filesystem::exists(folder) && std::filesystem::is_directory(folder)) {
                    WallpaperManager::add_folder(folder);
                    WallpaperChanger::refresh();
                    WallpaperChangerService::notify_added_wallpaper();
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
            // auto [type, c] = Input::get_input_key();
            if (Input::is_key_up() and choice > 0) choice--;
            else if (Input::is_key_down() and choice < WallpaperManager::get_folders().size() - 1) choice++;
            else if (Input::is_key_escape()) App::set_menu(App::MAIN_MENU);
            else if (Input::is_key_enter()) {
                auto& folder = WallpaperManager::get_folders()[choice];
                // CommandManager::Remove::run(folder);
                WallpaperManager::remove_folder(folder);
                WallpaperChanger::refresh();
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