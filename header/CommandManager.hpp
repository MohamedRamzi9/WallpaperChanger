

#pragma once

#include <string>
#include <vector>
#include "rmz_type.hpp"         
#include "../header/WallpaperChanger.hpp"


using rmz::type::nullable;

class CommandManager {
public:
    static void initialize();
    static std::vector<std::string> get_strings();
    static std::vector<std::string> get_descriptions();

    struct Duration {
        static std::string get_string(const WallpaperChanger::duration_type& duration);
        static std::string get_string();
        static std::string get_description();
        static nullable<WallpaperChanger::duration_type> parse(const std::string& input);
    };

    struct Add {
        static std::string get_string(const std::vector<std::string>& folders);
        static std::string get_string();
        static std::string get_description();
        static nullable<std::string> parse(const std::string& input);
    };

    struct Order {
        static std::string get_string();
        static std::string get_description();
        static bool parse(const std::string& input);
    };

    struct Random {
        static std::string get_string();
        static std::string get_description();
        static bool parse(const std::string& input);
    };

    struct Pause {
        static std::string get_string();
        static std::string get_description();
        static bool parse(const std::string& input);
    };

    struct Resume {
        static std::string get_string();
        static std::string get_description();
        static bool parse(const std::string& input);
    };

    struct Next {
        static std::string get_string();
        static std::string get_description();
        static bool parse(const std::string& input);
    };

    struct Previous {
        static std::string get_string();
        static std::string get_description();
        static bool parse(const std::string& input);
    };

    struct Set {
        static std::string get_string();
        static std::string get_description();
        static nullable<std::string> parse(const std::string& input);
    };

    struct Help {
        static std::string get_string();
        static std::string get_description();
        static bool parse(const std::string& input);
    };

    struct Clear {
        enum type { HELP, PARAMETERS, ERROR, ALL };
        static std::string get_string();
        static std::string get_description();
        static nullable<type> parse(const std::string& input);

        static bool is_clear_help(type clear_type);
        static bool is_clear_parameters(type clear_type);
        static bool is_clear_error(type clear_type);
        static bool is_clear_all(type clear_type);
    };

    struct Parameters {
        static std::string get_string();
        static std::string get_description();
        static bool parse(const std::string& input);
    };

    struct SaveFile {
        static std::string get_string();
        static std::string get_string(const std::string& path);
        static std::string get_description();
        static nullable<std::string> parse(const std::string& input);
    };

    struct Save {
        static std::string get_string();
        static std::string get_description();
        static bool parse(const std::string& input);
    };
    
    struct AutoSave {
        static std::string get_string();
        static std::string get_string(bool on);
        static std::string get_description();
        static nullable<bool> parse(const std::string& input);
    };

    struct Load {
        static std::string get_string();
        static std::string get_description();
        static nullable<std::string> parse(const std::string& input);
    };

    struct Exit {
        static std::string get_string();
        static std::string get_description();
        static bool parse(const std::string& input);
    };

private:
    static std::vector<std::string> strings;
    static std::vector<std::string> descriptions;
};
