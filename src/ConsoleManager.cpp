

#include "ConsoleManager.hpp"
#include "CommandManager.hpp"
#include "WallpaperManager.hpp"
#include "Utility.hpp"
#include "Other.hpp"

#include <ranges>

#include "rmz_console.hpp"
#include "rmz_print.hpp"


std::atomic<bool> ConsoleManager::signaled(false);
std::string ConsoleManager::wallpaper_changed_message;
std::string ConsoleManager::error_message;
bool ConsoleManager::show_help_flag = false;
bool ConsoleManager::show_parameters_flag = false;

void ConsoleManager::initialize() { rmz::enable_ansi(); }

void ConsoleManager::show_help() { show_help_flag = true; }
void ConsoleManager::clear_help() { show_help_flag = false; }
void ConsoleManager::show_parameters() { show_parameters_flag = true; }
void ConsoleManager::clear_parameters() { show_parameters_flag = false; }

void ConsoleManager::set_wallpaper_changed_message(const std::string& message) { wallpaper_changed_message = message; }
void ConsoleManager::set_error_message(const std::string& message) { error_message = message; }
void ConsoleManager::add_error_message(const std::string& message) { error_message += message + '\n'; }

void ConsoleManager::signal_update() {
    signaled.store(true);
    rmz::insert_enter();
}
void ConsoleManager::clear_signal() { signaled.store(false); }
void ConsoleManager::clear_error_message() { error_message.clear(); }
bool ConsoleManager::is_signaled() { return signaled.load(); }

void ConsoleManager::update() {
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

std::string ConsoleManager::get_input() {
    std::string raw_input;
    std::getline(std::cin, raw_input);
    while (ConsoleManager::is_signaled()) {
        ConsoleManager::clear_signal();
        ConsoleManager::update();
        rmz::insert_input(raw_input);
        std::getline(std::cin, raw_input);
    }
    return raw_input;
}

void ConsoleManager::print_commands() {
    rmz::println("* Commands: {}", join_string(CommandManager::get_strings(), ", "));
}

void ConsoleManager::print_help() {
    rmz::println("* Available commands:");
    for (auto [command, description] : std::views::zip(CommandManager::get_strings(), CommandManager::get_descriptions())) {
        rmz::println("  - '{}': {}", command, description);
    }
}

void ConsoleManager::print_parameters() {
    rmz::println("* Current parameters:");
    rmz::println("  - Duration: {}s", WallpaperChanger::get_duration().count());
    rmz::println("  - Change order: {}", WallpaperChanger::is_change_order_order() ? "Order" : "Random");
    rmz::println("  - Automatic wallpaper changer state: {}", 
        state.load() == RUNNING ? "Running" : (state.load() == PAUSED ? "Paused" : "Stopped"));
    rmz::println("  - Folders: {}", join_string(WallpaperManager::get_folders(), ", "));
    rmz::println("  - Auto-save: {}", auto_save ? "On" : "Off");
}
