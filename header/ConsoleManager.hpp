
#pragma once

#include <atomic>
#include <string>


struct ConsoleManager {
    static std::atomic<bool> signaled;
    static std::string wallpaper_changed_message;
    static std::string error_message;
    static bool show_help_flag;
    static bool show_parameters_flag;

    static void initialize();
    static void show_help();
    static void clear_help();
    static void show_parameters();
    static void clear_parameters();
    static void set_wallpaper_changed_message(const std::string& message);
    static void set_error_message(const std::string& message);
    static void add_error_message(const std::string& message);
    static void signal_update();
    static void clear_signal();
    static void clear_error_message();
    static bool is_signaled();
    static void update();
    static std::string get_input();

private:
    static void print_commands();
    static void print_help();
    static void print_parameters();
};
