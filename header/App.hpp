
#pragma once


#include <string>

#include "rmz_type.hpp"

enum input_enum { UP, DOWN, LEFT, RIGHT, CHARACTER, ENTER };
using input_type = std::pair<input_enum,char>;
rmz::type::nullable<input_type> get_input();


namespace App {
    enum menu_type { MAIN_MENU, DURATION_MENU } extern menu;

    extern std::string error_message;
    extern std::string info_message;
    extern std::string wallpaper_changed_message;
    extern bool show_parameters_flag;

    void initialize();
    void set_error_message(const std::string& message);
    void set_info_message(const std::string& message);
    void set_wallpaper_changed_message(const std::string& message);
    void show_parameters();
    void signal();
    void clear_error_message();
    void clear_info_message();
    void clear_parameters();
    void clear_wallpaper_changed_message();

    void clear_console();
    std::string get_input_string();
    void set_menu(menu_type new_menu);


    namespace MainMenu {
        extern int choice;
        void initialize();
        void update();
        void render();
    }

    namespace DurationMenu {
        void initialize();
        void update();
        void render();
    }

    void render();
    void update(auto input);
    void run();
}