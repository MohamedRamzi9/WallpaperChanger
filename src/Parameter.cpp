#include "Parameter.hpp"
#include "WallpaperChanger.hpp"
#include "WallpaperManager.hpp"
#include "CommandManager.hpp"
#include "Utility.hpp"
#include "Global.hpp"

namespace Parameters {
    void initialize() {}

    void load(const std::string& name) {
        auto parameters = read(name);
        for (auto& parameter : parameters) {
            if (!action(parameter)) {
            }
        }
    }


    void save(const std::string& name) {
        std::ofstream file(name, std::ios::trunc);
        write_line(file, CommandManager::Duration::get_string(WallpaperChanger::get_duration()));
        
        write_line(file, CommandManager::Add::get_string_wallpapers(WallpaperManager::get_alone_wallpapers()));
        std::vector<std::string> folders;
        for (auto it = WallpaperManager::get_folders().begin() + 1; it != WallpaperManager::get_folders().end(); ++it) {
            folders.push_back(it->path);
        }
        write_line(file, CommandManager::Add::get_string_folders(folders));
        
        write_line(file, WallpaperChanger::is_change_order_sequential() 
            ? CommandManager::Order::get_string() 
            : CommandManager::Random::get_string());
        
            write_line(file, CommandManager::AutoSave::get_string(auto_save));
        file.close();
    }

    void write_line(std::ofstream& file, const std::string& line) { file << line << '\n'; }

    std::vector<std::string> read(const std::string& name) {
        std::vector<std::string> result;
        std::ifstream file(name);
        std::string line;
        while (std::getline(file, line)) result.push_back(line);
        return result;
    }

}
