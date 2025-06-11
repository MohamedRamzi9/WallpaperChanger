

#pragma once

#include <string>
#include <vector>
#include "rmz_type.hpp"         
#include "../header/WallpaperChanger.hpp"


using rmz::type::nullable;

namespace CommandManager {


    enum command_enum { DURATION, ORDER, RANDOM, PAUSE, RESUME, NEXT, PREVIOUS, SET, ADD, EXIT, HELP, CLEAR, PARAMETERS, SAVEFILE, SAVE, AUTOSAVE };
    struct command_type {
        command_enum type;
        std::string string;
        std::string description;
    };
    using command_vector_type = std::vector<command_type>;
    
    void initialize();

    command_vector_type& get_commands();
    int get_command_count();
    command_type& get_command(int index);

    namespace Duration {
        std::string get_string(const WallpaperChanger::duration_type& duration);
        std::string get_string();
        std::string get_description();
        nullable<WallpaperChanger::duration_type> parse(const std::string& input);
        void run(const WallpaperChanger::duration_type& duration);
    };

    namespace Add {
        std::string get_string(const std::vector<std::string>& folders);
        std::string get_string();
        std::string get_description();
        nullable<std::string> parse(const std::string& input);
        void run(const std::string& folder);
    };

    namespace Order {
        std::string get_string();
        std::string get_description();
        bool parse(const std::string& input);
        void run();
    };

    namespace Random {
        std::string get_string();
        std::string get_description();
        bool parse(const std::string& input);
        void run();
    };

    namespace Pause {
        std::string get_string();
        std::string get_description();
        bool parse(const std::string& input);
        void run();
    };

    namespace Resume {
        std::string get_string();
        std::string get_description();
        bool parse(const std::string& input);
        void run();
    };

    namespace Next {
        std::string get_string();
        std::string get_description();
        bool parse(const std::string& input);
    };

    namespace Previous {
        std::string get_string();
        std::string get_description();
        bool parse(const std::string& input);
    };

    namespace Set {
        std::string get_string();
        std::string get_description();
        nullable<std::string> parse(const std::string& input);
    };

    namespace Help {
        std::string get_string();
        std::string get_description();
        bool parse(const std::string& input);
    };

    namespace Clear {
        enum type { HELP, PARAMETERS, ERROR, ALL };
        std::string get_string();
        std::string get_description();
        nullable<type> parse(const std::string& input);

        bool is_clear_help(type clear_type);
        bool is_clear_parameters(type clear_type);
        bool is_clear_error(type clear_type);
        bool is_clear_all(type clear_type);
    };

    namespace Parameters {
        std::string get_string();
        std::string get_description();
        bool parse(const std::string& input);
        void run();
    };

    namespace SaveFile {
        std::string get_string();
        std::string get_string(const std::string& path);
        std::string get_description();
        nullable<std::string> parse(const std::string& input);
    };

    namespace Save {
        std::string get_string();
        std::string get_description();
        bool parse(const std::string& input);
    };
    
    namespace AutoSave {
        std::string get_string();
        std::string get_string(bool on);
        std::string get_description();
        nullable<bool> parse(const std::string& input);
    };

    namespace Load {
        std::string get_string();
        std::string get_description();
        nullable<std::string> parse(const std::string& input);
    };

    namespace Exit {
        std::string get_string();
        std::string get_description();
        bool parse(const std::string& input);
    };

// private:

};
