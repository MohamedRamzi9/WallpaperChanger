#include "CommandManager.hpp"
#include "WallpaperManager.hpp"
#include "WallpaperChanger.hpp"
#include "WallpaperGetter.hpp"
#include "Global.hpp"
#include "Utility.hpp"
#include "App.hpp"

namespace CommandManager {

    command_vector_type commands;

    void initialize() {
        commands = {
            { DURATION, Duration::get_string(), Duration::get_description() },
            { ORDER, Order::get_string(), Order::get_description() },
            { RANDOM, Random::get_string(), Random::get_description() },
            { PAUSE, Pause::get_string(), Pause::get_description() },
            { RESUME, Resume::get_string(), Resume::get_description() },
            { NEXT, Next::get_string(), Next::get_description() },
            { PREVIOUS, Previous::get_string(), Previous::get_description() },
            { SET, Set::get_string(), Set::get_description() },
            { ADD, Add::get_string(), Add::get_description() },
            { EXIT, Exit::get_string(), Exit::get_description() },
            { HELP, Help::get_string(), Help::get_description() },
            { CLEAR, Clear::get_string(), Clear::get_description() },
            { PARAMETERS, Parameters::get_string(), Parameters::get_description() },
            { SAVEFILE, SaveFile::get_string(), SaveFile::get_description() },
            { SAVE, Save::get_string(), Save::get_description() },
            { AUTOSAVE, AutoSave::get_string(), AutoSave::get_description() }
        };
          
    }

    command_vector_type& get_commands() {
        return commands;
    }
    int get_command_count() {
        return commands.size();
    }
    command_type& get_command(int index) {
        return commands[index];
    }

    namespace Duration {       
        std::string get_string(const WallpaperChanger::duration_type& duration) { return std::format("{}s", duration.count()); }
        std::string get_string() { return "<int>s/m"; }
        std::string get_description() { return "Set the wallpaper change interval (e.g., '10s' for 10 seconds, '5m' for 5 minutes)."; }
        nullable<WallpaperChanger::duration_type> parse(const std::string& input) {
            if (input.empty()) return {};

            char last = input.back();
            if (last != 's' && last != 'm') return {};

            std::string numberPart = input.substr(0, input.size() - 1);

            if (numberPart.empty() || !std::all_of(numberPart.begin(), numberPart.end(), ::isdigit))
                return {};

            int value = std::stoi(numberPart);
            char unit = last;
            return {unit == 's' ? WallpaperChanger::duration_type(rmz::seconds(value)) : WallpaperChanger::duration_type(rmz::minutes(value))};
        }
        void run(const WallpaperChanger::duration_type& duration) {
            WallpaperChanger::set_duration(duration);
        }
    }

    namespace Add {
        std::string get_string(const std::vector<std::string>& folders) { return "add " + join_string(folders, " "); }
        std::string get_string() { return "add <folder>"; }
        std::string get_description() { return "Add a folder to the list of wallpaper folders and load its wallpapers."; }
        nullable<std::string> parse(const std::string& input) {
            auto parts = split_string(input, ' ');
            if (parts.size() < 2 || parts[0] != "add") return {};
            return parts[1];
        }
        void run(const std::string& folder) {
            WallpaperManager::add_folder(folder);
            WallpaperRandomGetter::refresh();
            WallpaperOrderGetter::refresh();
            empty_semaphore.release();
        }
    }

    namespace Order {
        std::string get_string() { return "order"; }
        std::string get_description() { return "Set wallpaper change order to sequential."; }
        bool parse(const std::string& input) { return input == "order"; }
        void run() { WallpaperChanger::set_change_order(WallpaperChanger::ORDER); }
    }

    namespace Random {
        std::string get_string() { return "random"; }
        std::string get_description() { return "Set wallpaper change order to random."; }
        bool parse(const std::string& input) { return input == "random"; }
        void run() { WallpaperChanger::set_change_order(WallpaperChanger::RANDOM); }
    }

    namespace Pause {
        std::string get_string() { return "pause"; }
        std::string get_description() { return "Pause the automatic wallpaper changer."; }
        bool parse(const std::string& input) { return input == "pause"; }
        void run() { state.store(PAUSED); }
    }

    namespace Resume {
        std::string get_string() { return "resume"; }
        std::string get_description() { return "Resume the automatic wallpaper changer."; }
        bool parse(const std::string& input) { return input == "resume"; }
        void run() { 
            state.store(RUNNING);
            pause_semaphore.release();
        }
    }

    namespace Next {
        std::string get_string() { return "next"; }
        std::string get_description() { return "Set the next wallpaper in the current order."; }
        bool parse(const std::string& input) { return input == "next"; }
    }

    namespace Previous {
        std::string get_string() { return "previous"; }
        std::string get_description() { return "Set the previous wallpaper in the current order."; }
        bool parse(const std::string& input) { return input == "previous"; }
    }

    namespace Set {
        std::string get_string() { return "set <path>"; }
        std::string get_description() { return "Set the wallpaper to a specific image file."; }
        nullable<std::string> parse(const std::string& input) {
        auto parts = split_string(input, ' ');
        if (parts.size() != 2 || parts[0] != "set") return {};
        return parts[1];
    }
    }

    namespace Help {
        std::string get_string() { return "help"; }
        std::string get_description() { return "Show this help message."; }
        bool parse(const std::string& input) { return input == "help"; }
    }

    namespace Clear {
        std::string get_string() { return "clear [help/parameters/errors]"; }
        std::string get_description() { return "Clear the help, parameters and error messages."; }

        nullable<type> parse(const std::string& input) {
            auto parts = split_string(input, ' ');
            if (parts[0] != "clear") return {};
            if (parts.size() == 1) return {ALL}; 
            if (parts.size() == 2) {
                if (parts[1] == "help") return {HELP};
                if (parts[1] == "parameters") return {PARAMETERS};
                if (parts[1] == "errors") return {ERROR};
            }
            return {};
        }
        bool is_clear_all(type clear_type) { return clear_type == ALL; }
        bool is_clear_help(type clear_type) { return clear_type == HELP; }
        bool is_clear_parameters(type clear_type) { return clear_type == PARAMETERS; }
        bool is_clear_error(type clear_type) { return clear_type == ERROR; }
    }

    namespace Parameters {
        std::string get_string() { return "parameters"; }
        std::string get_description() { return "Show the current parameters."; }
        bool parse(const std::string& input) { return input == "parameters"; }
        void run() { App::show_parameters(); }
    }

    namespace SaveFile {
        std::string get_string() { return "savefile [<file>|default]"; }
        std::string get_string(const std::string& path) { return "save " + path; }
        std::string get_description() { return "Save parameters to a file. If default is specified, save to 'settings.wallpaper'."; }
        nullable<std::string> parse(const std::string& input) {
            auto parts = split_string(input, ' ');
            if (parts.size() != 2 || parts[0] != "savefile") return {};
            if (parts[1] == "default") return {"settings.wallpaper"};
            return {parts[1]};
        }
    }

    namespace Save {
        std::string get_string() { return "save"; }
        std::string get_description() { return "Save the current parameters to the specified file."; }
        bool parse(const std::string& input) { return input == "save"; }
    }

    namespace AutoSave {
        std::string get_string() { return "autosave <on|off>"; }
        std::string get_string(bool on) { return "autosave " + std::string(on ? "on" : "off"); }
        std::string get_description() { return "Enable or disable automatic saving of parameters when exiting the program."; }
        nullable<bool> parse(const std::string& input) {
            auto parts = split_string(input, ' ');
            if (parts.size() != 2 || parts[0] != "autosave") return {};
            if (parts[1] == "on") return {true};
            if (parts[1] == "off") return {false};
            return {};
        }
    }        

    namespace Load {
        std::string get_string() { return "load [<file>]"; }
        std::string get_description() { return "Load parameters from a file the specified file, or the default file if none is provided."; }
        nullable<std::string> parse(const std::string& input) {
            auto parts = split_string(input, ' ');
            if (parts.empty() || parts[0] != "load") return {};
            if (parts.size() == 2) return parts[1];
            return {"settings.wallpaper"};
        }
    }

    namespace Exit {
        std::string get_string() { return "exit"; }
        std::string get_description() { return "Exit the program and stop the automatic wallpaper changer."; }
        bool parse(const std::string& input) { return input == "exit"; }
    
    }

}