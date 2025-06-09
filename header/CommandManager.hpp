

#pragma once

#include <string>
#include <vector>
#include "rmz_type.hpp"         
#include "../header/WallpaperChanger.hpp"


using rmz::type::nullable;

namespace CommandManager {
// public:
    extern std::vector<std::string> strings;
    extern std::vector<std::string> descriptions;
    
    void initialize();
    std::vector<std::string> get_strings();
    std::vector<std::string> get_descriptions();

    namespace Duration {
        std::string get_string(const WallpaperChanger::duration_type& duration);
        std::string get_string();
        std::string get_description();
        nullable<WallpaperChanger::duration_type> parse(const std::string& input);
    };

    namespace Add {
        std::string get_string(const std::vector<std::string>& folders);
        std::string get_string();
        std::string get_description();
        nullable<std::string> parse(const std::string& input);
    };

    namespace Order {
        std::string get_string();
        std::string get_description();
        bool parse(const std::string& input);
    };

    namespace Random {
        std::string get_string();
        std::string get_description();
        bool parse(const std::string& input);
    };

    namespace Pause {
        std::string get_string();
        std::string get_description();
        bool parse(const std::string& input);
    };

    namespace Resume {
        std::string get_string();
        std::string get_description();
        bool parse(const std::string& input);
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
