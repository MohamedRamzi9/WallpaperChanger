
#pragma once

#include <atomic>
#include <string>


namespace ConsoleManager {
    extern std::atomic<bool> signaled;
    extern std::string wallpaper_changed_message;
    extern std::string error_message;
    extern bool show_help_flag;
    extern bool show_parameters_flag;
    extern int cursor_column;

    void initialize();
    void show_help();
    void clear_help();
    void show_parameters();
    void clear_parameters();
    void set_wallpaper_changed_message(const std::string& message);
    void set_error_message(const std::string& message);
    void add_error_message(const std::string& message);
    void signal_update();
    void clear_signal();
    void clear_error_message();
    bool is_signaled();
    void update();
    std::string get_input();

// private:
    void print_commands();
    void print_help();
    void print_parameters();
};
