#include "CommandManager.hpp"
#include "Utility.hpp"


std::vector<std::string> CommandManager::strings;
std::vector<std::string> CommandManager::descriptions;

void CommandManager::initialize() {
    strings = {
        Duration::get_string(),
        Order::get_string(),
        Random::get_string(),
        Pause::get_string(),
        Resume::get_string(),
        Next::get_string(),
        Previous::get_string(),
        Set::get_string(),
        Add::get_string(),
        Exit::get_string(),
        Help::get_string(),
        Clear::get_string(),
        Parameters::get_string(),
    };
    descriptions = {
        Duration::get_description(),
        Order::get_description(),
        Random::get_description(),
        Pause::get_description(),
        Resume::get_description(),
        Next::get_description(),
        Previous::get_description(),
        Set::get_description(),
        Add::get_description(),
        Exit::get_description(),
        Help::get_description(),
        Clear::get_description(),
        Parameters::get_description(),
    };
}


std::vector<std::string> CommandManager::get_strings() { return strings; }
std::vector<std::string> CommandManager::get_descriptions() { return descriptions; }


std::string CommandManager::Duration::get_string(const WallpaperChanger::duration_type& duration) { return std::format("{}s", duration.count()); }
std::string CommandManager::Duration::get_string() { return "<int>s/m"; }
std::string CommandManager::Duration::get_description() { return "Set the wallpaper change interval (e.g., '10s' for 10 seconds, '5m' for 5 minutes)."; }
nullable<WallpaperChanger::duration_type> CommandManager::Duration::parse(const std::string& input) {
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


std::string CommandManager::Add::get_string(const std::vector<std::string>& folders) { return "add " + join_string(folders, " "); }
std::string CommandManager::Add::get_string() { return "add <folder>"; }
std::string CommandManager::Add::get_description() { return "Add a folder to the list of wallpaper folders and load its wallpapers."; }
nullable<std::string> CommandManager::Add::parse(const std::string& input) {
    auto parts = split_string(input, ' ');
    if (parts.size() < 2 || parts[0] != "add") return {};
    return parts[1];
}


std::string CommandManager::Order::get_string() { return "order"; }
std::string CommandManager::Order::get_description() { return "Set wallpaper change order to sequential."; }
bool CommandManager::Order::parse(const std::string& input) { return input == "order"; }


std::string CommandManager::Random::get_string() { return "random"; }
std::string CommandManager::Random::get_description() { return "Set wallpaper change order to random."; }
bool CommandManager::Random::parse(const std::string& input) { return input == "random"; }


std::string CommandManager::Pause::get_string() { return "pause"; }
std::string CommandManager::Pause::get_description() { return "Pause the automatic wallpaper changer."; }
bool CommandManager::Pause::parse(const std::string& input) { return input == "pause"; }


std::string CommandManager::Resume::get_string() { return "resume"; }
std::string CommandManager::Resume::get_description() { return "Resume the automatic wallpaper changer."; }
bool CommandManager::Resume::parse(const std::string& input) { return input == "resume"; }


std::string CommandManager::Next::get_string() { return "next"; }
std::string CommandManager::Next::get_description() { return "Set the next wallpaper in the current order."; }
bool CommandManager::Next::parse(const std::string& input) { return input == "next"; }


std::string CommandManager::Previous::get_string() { return "previous"; }
std::string CommandManager::Previous::get_description() { return "Set the previous wallpaper in the current order."; }
bool CommandManager::Previous::parse(const std::string& input) { return input == "previous"; }


std::string CommandManager::Set::get_string() { return "set <path>"; }
std::string CommandManager::Set::get_description() { return "Set the wallpaper to a specific image file."; }
nullable<std::string> CommandManager::Set::parse(const std::string& input) {
    auto parts = split_string(input, ' ');
    if (parts.size() != 2 || parts[0] != "set") return {};
    return parts[1];
}


std::string CommandManager::Help::get_string() { return "help"; }
std::string CommandManager::Help::get_description() { return "Show this help message."; }
bool CommandManager::Help::parse(const std::string& input) { return input == "help"; }


std::string CommandManager::Clear::get_string() { return "clear [help/parameters/errors]"; }
std::string CommandManager::Clear::get_description() { return "Clear the help, parameters and error messages."; }


nullable<CommandManager::Clear::type> CommandManager::Clear::parse(const std::string& input) {
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
bool CommandManager::Clear::is_clear_all(CommandManager::Clear::type clear_type) { return clear_type == ALL; }
bool CommandManager::Clear::is_clear_help(CommandManager::Clear::type clear_type) { return clear_type == HELP; }
bool CommandManager::Clear::is_clear_parameters(CommandManager::Clear::type clear_type) { return clear_type == PARAMETERS; }
bool CommandManager::Clear::is_clear_error(CommandManager::Clear::type clear_type) { return clear_type == ERROR; }


std::string CommandManager::Parameters::get_string() { return "parameters"; }
std::string CommandManager::Parameters::get_description() { return "Show the current parameters."; }
bool CommandManager::Parameters::parse(const std::string& input) { return input == "parameters"; }


std::string CommandManager::SaveFile::get_string() { return "save [<file>|default]"; }
std::string CommandManager::SaveFile::get_string(const std::string& path) { return "save " + path; }
std::string CommandManager::SaveFile::get_description() { return "Save parameters to a file. If default is specified, save to 'settings.wallpaper'."; }
nullable<std::string> CommandManager::SaveFile::parse(const std::string& input) {
    auto parts = split_string(input, ' ');
    if (parts.size() != 2 or parts[0] != "savefile") return {};
    if (parts[1] == "default") return {"settings.wallpaper"};
    return {parts[1]};
}


std::string CommandManager::Save::get_string() { return "save"; }
std::string CommandManager::Save::get_description() { return "Save the current parameters to the specified file."; }
bool CommandManager::Save::parse(const std::string& input) { return input == "save"; }


std::string CommandManager::AutoSave::get_string() { return "autosave <on/off>"; }
std::string CommandManager::AutoSave::get_string(bool on) { return "autosave " + std::string(on ? "on" : "off"); }
std::string CommandManager::AutoSave::get_description() { return "Enable or disable automatic saving of parameters when exiting the program."; }
nullable<bool> CommandManager::AutoSave::parse(const std::string& input) {
    auto parts = split_string(input, ' ');
    if (parts.size() != 2 || parts[0] != "autosave") return {};
    if (parts[1] == "on") return {true};
    if (parts[1] == "off") return {false};
    return {};
}
    

std::string CommandManager::Load::get_string() { return "load <file>"; }
std::string CommandManager::Load::get_description() { return "Load parameters from a file."; }
nullable<std::string> CommandManager::Load::parse(const std::string& input) {
    auto parts = split_string(input, ' ');
    if (parts.size() != 2 || parts[0] != "load") return {};
    return parts[1];
}


std::string CommandManager::Exit::get_string() { return "exit"; }
std::string CommandManager::Exit::get_description() { return "Exit the program and stop the automatic wallpaper changer."; }
bool CommandManager::Exit::parse(const std::string& input) { return input == "exit"; }
