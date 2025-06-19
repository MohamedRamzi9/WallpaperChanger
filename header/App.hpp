
#pragma once


#include <windows.h>
#include <string>
#include <atomic>

#include "rmz_type.hpp"


namespace App {
    enum menu_type { MAIN_MENU, DURATION_MENU, ADD_MENU, REMOVE_MENU, SET_WALLPAPER_MENU} extern menu;
    // enum input_key_enum { UP, DOWN, LEFT, RIGHT, CHARACTER, ENTER, ESCAPE };
    // using input_key_type = std::pair<input_key_enum,char>;

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
    void set_menu(menu_type new_menu);

    namespace Input {
        enum input_enum { STRING, KEY };
        extern INPUT_RECORD input_key;
        // enum input_key_enum { UP=VK_UP, DOWN=VK_DOWN, LEFT=VK_LEFT, RIGHT=VK_RIGHT, CHARACTER=VK_RETURN, ENTER=VK_RETURN, ESCAPE=VK_ESCAPE };
        // using input_key_type = std::pair<input_key_enum,char>;
        

        void initialize();
        void set_input_key();
        void set_input_string();

        bool is_getting_input();
        bool is_input_string();
        bool is_input_key();

        void stop_getting_input();
        void insert_input(const std::string& input);

        void update_input_key();
        void update_input_string();
        char get_char();
        std::string& get_input_string();
        // template <WORD VirtualKeyCode> bool is_key_type();
        bool is_key_escape();
        bool is_key_up();
        bool is_key_down(); 
        bool is_key_left();
        bool is_key_right();
        bool is_key_enter();
        bool is_key_char();


    }


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

    namespace AddMenu {
        void initialize();
        void update();
        void render();
    }

    namespace RemoveMenu {
        void initialize();
        void update();
        void render();
    }

    namespace SetWallpaperMenu {
        void initialize();
        void update();
        void render();
    }

    void render();
    void update(auto input);
    void run();
}