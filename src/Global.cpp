#include "Global.hpp"
#include "CommandManager.hpp"
#include "WallpaperChanger.hpp"
#include "WallpaperManager.hpp"
#include "Parameter.hpp"

std::counting_semaphore<0> pause_semaphore(0);
std::counting_semaphore<0> empty_semaphore(0);
std::atomic<State> state{RUNNING};
std::string save_file = "settings.wallpaper";
bool auto_save = true;



bool action(const std::string& input) {
    bool valid_command = true;
    
    if (CommandManager::Order::parse(input)) {
        CommandManager::Order::run();

    } else if (CommandManager::Random::parse(input)) {
        CommandManager::Random::run();
        
    } else if (auto result = CommandManager::Duration::parse(input)) {
        WallpaperChanger::set_duration(result.get());

    } else if (CommandManager::Pause::parse(input)) {
        CommandManager::Pause::run();

    } else if (auto result = CommandManager::Add::parse(input)) {
        CommandManager::Add::run(result.get());

    } else if (CommandManager::Resume::parse(input)) {
        CommandManager::Resume::run();

    } else if (CommandManager::Next::parse(input)) {
        WallpaperChanger::set_next_wallpaper();

    } else if (CommandManager::Previous::parse(input)) {
        WallpaperChanger::set_previous_wallpaper();

    } else if (auto result = CommandManager::Set::parse(input)) {
        WallpaperChanger::set_wallpaper(result.get());

    } else if (CommandManager::Help::parse(input)) {

    } else if (CommandManager::Parameters::parse(input)) {

    // } else if (auto result = CommandManager::SaveFile::parse(input)) {
    //     save_file = result.get();

    } else if (CommandManager::Save::parse(input)) {
        Parameters::save(save_file);
        
    } else if (auto result = CommandManager::AutoSave::parse(input)) {
        auto_save = result.get();
    
    } else if (auto result = CommandManager::Load::parse(input)) {
        Parameters::load(result.get());

    } else if (auto result = CommandManager::Clear::parse(input)) {
        auto parameter = result.get();
        if (CommandManager::Clear::is_clear_all(parameter)) {
        } else if (CommandManager::Clear::is_clear_help(parameter)) {
        } else if (CommandManager::Clear::is_clear_parameters(parameter)) {
        } else if (CommandManager::Clear::is_clear_error(parameter)) {
        }

    } else {
        valid_command = false;
    }

    if (valid_command) {
    }
    
    return valid_command;
}
