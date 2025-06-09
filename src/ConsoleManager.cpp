

#include "ConsoleManager.hpp"
#include "CommandManager.hpp"
#include "WallpaperManager.hpp"
#include "Utility.hpp"
#include "Other.hpp"

#include <ranges>

#include "rmz_console.hpp"
#include "rmz_print.hpp"

namespace ConsoleManager {
    std::atomic<bool> signaled(false);
    std::string wallpaper_changed_message;
    std::string error_message;
    bool show_help_flag = false;
    bool show_parameters_flag = false;
    int cursor_column = 0;

    void initialize() { rmz::enable_ansi(); }

    void show_help() { show_help_flag = true; }
    void clear_help() { show_help_flag = false; }
    void show_parameters() { show_parameters_flag = true; }
    void clear_parameters() { show_parameters_flag = false; }

    void set_wallpaper_changed_message(const std::string& message) { wallpaper_changed_message = message; }
    void set_error_message(const std::string& message) { error_message = message; }
    void add_error_message(const std::string& message) { error_message += message + '\n'; }

    void signal_update() {
        signaled.store(true);
        rmz::insert_enter();
        // get the column position of the cursor
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            cursor_column = csbi.dwCursorPosition.X;
        }
    }
    void clear_signal() { signaled.store(false); }
    void clear_error_message() { error_message.clear(); }
    bool is_signaled() { return signaled.load(); }

    void update() {
        rmz::clear_console();
        if (!wallpaper_changed_message.empty()) {
            rmz::println(wallpaper_changed_message);
            rmz::println();
        }
        if (!error_message.empty()) {
            rmz::println(error_message);
            rmz::println();
        }
        if (show_parameters_flag) {
            print_parameters();
            rmz::println();
        }
        if (show_help_flag) {
            print_help();
            rmz::println();
        } else {
            print_commands();
            rmz::println();
        }
    }

    std::string get_input() {
        std::string raw_input;
        std::getline(std::cin, raw_input);
        while (is_signaled()) {
            clear_signal();
            update();
            rmz::insert_input(raw_input);
            std::getline(std::cin, raw_input);
        }
        return raw_input;
    }

    void print_commands() {
        std::vector<std::string> strings;
        for (const auto& command : CommandManager::get_commands())
            strings.push_back(command.string);

        rmz::println("* Commands: {}", join_string(strings, ", "));
    }

    void print_help() {
        rmz::println("* Available commands:");
        for (const auto& command : CommandManager::get_commands()) {
            rmz::println("  - '{}': {}", command.string, command.description);
        }
    }

    void print_parameters() {
        rmz::println("* Current parameters:");
        rmz::println("  - Duration: {}s", WallpaperChanger::get_duration().count());
        rmz::println("  - Change order: {}", WallpaperChanger::is_change_order_order() ? "Order" : "Random");
        rmz::println("  - Automatic wallpaper changer state: {}", 
            state.load() == RUNNING ? "Running" : (state.load() == PAUSED ? "Paused" : "Stopped"));
        rmz::println("  - Folders: {}", join_string(WallpaperManager::get_folders(), ", "));
        rmz::println("  - Auto-save: {}", auto_save ? "On" : "Off");
    }
}