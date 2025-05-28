
#include <windows.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <thread>
#include <numeric>
#include <algorithm>
#include <random>
#include <atomic>
#include <ranges>
#include <semaphore>
#include <fstream>

#include "rmz_console.hpp"
#include "rmz_type.hpp"
#include "rmz_time.hpp"
#include "rmz_print.hpp"
#include "rmz_algorithm.hpp"
// #include "rmz_file.hpp"


// ===========================================
// ============ UTILITY FUNCTIONS ============
// ===========================================

std::vector<std::string> split_string(const std::string& str, char delimiter) {
    std::vector<std::string> result;

    for (auto &&part : std::views::split(str, delimiter)) {
        result.emplace_back(part.begin(), part.end());
    }

    return result;
}

std::string join_string(const std::vector<std::string>& vec, char delimiter) {
    if (vec.empty()) return "";
    std::string result = vec[0];
    for (size_t i = 1; i < vec.size(); ++i) {
        result += delimiter + vec[i];
    }
    return result;
}

rmz::type::nullable<std::chrono::seconds> parse_duration(const std::string& input) {
    if (input.empty()) return {};

    // Check last character is 's' or 'm'
    char last = input.back();
    if (last != 's' && last != 'm') return {};

    // Extract numeric part
    std::string numberPart = input.substr(0, input.size() - 1);

    // Check if all characters in numberPart are digits
    if (numberPart.empty() || !std::all_of(numberPart.begin(), numberPart.end(), ::isdigit))
        return {};

    int value; char unit;
    value = std::stoi(numberPart);
    unit = last;
    return {unit == 's' ? std::chrono::seconds(value) : std::chrono::minutes(value)};
}




// ==========================================
// ============ GLOBAL VARIABLES ============
// ==========================================

std::counting_semaphore<0> pause_semaphore(0);
std::counting_semaphore<0> empty_semaphore(0);
enum State { RUNNING, PAUSED, STOPPED }; std::atomic<State> state;
using duration_type = rmz::seconds; rmz::timer timer(duration_type(10)); 



// ======================================
// ============ DECLARATIONS ============
// ======================================

bool action(const std::vector<std::string>& input);

struct WallpaperManager {
    static std::vector<std::string> folders;
    static std::vector<std::string> wallpapers;
    
    static void initialize() {
        load_all_wallpapers();
    }
    static void add_folder(const std::string& folder) {
        if (std::filesystem::exists(folder) && std::filesystem::is_directory(folder)) {
            folders.push_back(folder);
            load_wallpapers(folder);
        } else {
            std::cerr << "Folder does not exist or is not a directory: " << folder << '\n';
        }
    }
    static bool empty() {
        return wallpapers.empty();
    }
    static void load_wallpapers(const std::string& folder) {
        try {
            for (const auto& entry : std::filesystem::directory_iterator(folder)) {
                if (entry.is_regular_file()) {
                    std::string path = entry.path().string();
                    std::string ext = entry.path().extension().string();

                    // Convert extension to lowercase for comparison
                    for (auto& c : ext) c = static_cast<char>(tolower(c));

                    if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp") {
                        wallpapers.push_back(path);
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << '\n';
        }
    }
    static void load_all_wallpapers() {
        wallpapers.clear();
        for (const auto& folder : folders) {
            load_wallpapers(folder);
        }
    }

    static int get_wallpaper_count() {
        return wallpapers.size();
    }

    static std::string get_wallpaper(int index) {
        return wallpapers[index];
    }
};
std::vector<std::string> WallpaperManager::folders;
std::vector<std::string> WallpaperManager::wallpapers;


struct WallpaperRandomGetter {
    static int current_index;
    static std::vector<int> indices;
    static std::mt19937 rng;
    static void initialize() {
        rng = std::mt19937(std::random_device()());
        
        current_index = 0;
        indices.resize(WallpaperManager::get_wallpaper_count());
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), rng);
    }
    static void refresh() {
        indices.resize(WallpaperManager::get_wallpaper_count());
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), rng);
    }
    static void reset() {
        current_index = 0;
    }

    static std::string get_next_wallpaper() {
        current_index++;
        if (current_index >= indices.size()) 
            current_index = 0;
        return WallpaperManager::get_wallpaper(indices[current_index]);
    }
    static std::string get_previous_wallpaper() {
        current_index--;
        if (current_index < 0) 
            current_index += indices.size();
        return WallpaperManager::get_wallpaper(indices[current_index]);
    }
};
int WallpaperRandomGetter::current_index;
std::vector<int> WallpaperRandomGetter::indices;
std::mt19937 WallpaperRandomGetter::rng;


struct WallpaperOrderGetter {
    static int current_index;

    static void initialize() {
        current_index = 0;
    }
    static void refresh() {}
    static void reset() {
        current_index = 0;
    }
    static std::string get_next_wallpaper() {
        current_index++;
        if (current_index >= WallpaperManager::get_wallpaper_count()) 
            current_index = 0;
        return WallpaperManager::get_wallpaper(current_index);
    }
    static std::string get_previous_wallpaper() {
        current_index--;
        if (current_index < 0)
            current_index += WallpaperManager::get_wallpaper_count();
        return WallpaperManager::get_wallpaper(current_index);
    }
};
int WallpaperOrderGetter::current_index;


struct WallpaperChanger {
    static enum change_order { ORDER, RANDOM } change_order;
    static void initialize() {
        change_order = ORDER; // Default order
    }

    static void set_change_order(enum change_order new_order) {
        change_order = new_order;
    }
    static void set_wallpaper(const std::string& path) {
        SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (PVOID)path.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
    }
    static std::string set_next_wallpaper() {
        std::string wallpaper;
        if (change_order == ORDER) {
            wallpaper = WallpaperOrderGetter::get_next_wallpaper();
        } else {
            wallpaper = WallpaperRandomGetter::get_next_wallpaper();
        }
        set_wallpaper(wallpaper);
        return wallpaper;
    }
    static std::string set_previous_wallpaper() {
        std::string wallpaper;
        if (change_order == ORDER) {
            wallpaper = WallpaperOrderGetter::get_previous_wallpaper();
        } else {
            wallpaper = WallpaperRandomGetter::get_previous_wallpaper();
        }
        set_wallpaper(wallpaper);
        return wallpaper;
    }

};
enum WallpaperChanger::change_order WallpaperChanger::change_order;


struct ConsoleManager {

    static std::atomic<bool> signaled;
    static std::string wallpaper_changed_message;
    static std::string error_message;

    static void initialize() { rmz::enable_ansi(); }

    static void print_options() {
        rmz::println("* Commands: 'exit', 'order', 'random', 'pause', 'resume', '<int><s|m>, 'next', 'previous', 'set <path>', 'add <folder>'");
    }

    static void set_wallpaper_changed_message(const std::string& message) {
        wallpaper_changed_message = message;
    }
    static void set_error_message(const std::string& message) {
        error_message = message;
    }
    static void add_error_message(const std::string& message) {
        if (error_message.empty()) {
            error_message = message;
        } else {
            error_message += '\n' + message;
        }
    }

    static void signal_update() {
        signaled.store(true);
        rmz::insert_enter();
    }
    static void clear_signal() {
        signaled.store(false);
    }
    static void clear_error_message() {
        error_message.clear();
    }
    static bool is_signaled() {
        return signaled.load();
    }
    
    static void update() {
        rmz::clear_console();
        if (not wallpaper_changed_message.empty()) {
            rmz::println(wallpaper_changed_message);
            rmz::println();
        }
        if (not error_message.empty()) {
            rmz::println(error_message);
            rmz::println();
        }
        print_options();
    }

    static std::vector<std::string> get_input() {
        std::string raw_input;
        std::getline(std::cin, raw_input);
        while (ConsoleManager::is_signaled()) {
            ConsoleManager::clear_signal();
            ConsoleManager::update();
            rmz::insert_input(raw_input);
            std::getline(std::cin, raw_input);
        }   
        return split_string(raw_input, ' ');
    }
};
std::atomic<bool> ConsoleManager::signaled(false);
std::string ConsoleManager::wallpaper_changed_message;
std::string ConsoleManager::error_message;


struct Parameters {
    
    static void initialize() {}

    static void load(const std::string& name) {
        auto parameters = read(name);
        for (auto& parameter : parameters) {
            if (not action(parameter)) {
                ConsoleManager::add_error_message(std::format("* Command '{}' not recognized in file '{}'", parameter[0], name));
            }
        }
    }

    static void save(const std::string& name) {
        std::ofstream file(name, std::ios::trunc);

        write_line(file, std::format("{}s", duration_type(timer.get_duration()).count()));
        write_line(file, std::format("add {}", join_string(WallpaperManager::folders, ' ')));
        write_line(file, std::format("{}", WallpaperChanger::change_order == WallpaperChanger::ORDER ? "order" : "random"));
        
        file.close();
    }

private:
    static void write_line(std::ofstream& file, const std::string& line) {
        file << line << '\n';
    }
    static std::vector<std::vector<std::string>> read(const std::string& name) {
        std::vector<std::vector<std::string>> result;
        std::ifstream file(name);

        std::string line;
        while (std::getline(file, line)) {
            std::vector<std::string> tokens = split_string(line, ' ');
            result.push_back(tokens);
        }
        return result;
    }

};



bool action(const std::vector<std::string>& input) {
    bool valid_command = true;
    std::string command = input[0];
    
    if (command == "order") {
        WallpaperChanger::set_change_order(WallpaperChanger::ORDER);
        
    } else if (command == "random") {
        WallpaperChanger::set_change_order(WallpaperChanger::RANDOM);
        
    } else if (auto result = parse_duration(command)) {
        timer.set_duration(result.get());
        timer.init();

    } else if (command == "pause") {
        state.store(PAUSED);

    } else if (command == "add") {
        std::string folder = input[1];
        WallpaperManager::add_folder(folder);
        WallpaperRandomGetter::refresh();
        WallpaperOrderGetter::refresh();
        empty_semaphore.release();

    } else if (command == "resume") {
        state.store(RUNNING);
        pause_semaphore.release();

        
    } else if (command == "next") {
        WallpaperChanger::set_next_wallpaper();
        
    } else if (command == "previous") {
        WallpaperChanger::set_previous_wallpaper();
        
    } else if (command == "set") {
        auto wallpaper = input[1];
        WallpaperChanger::set_wallpaper(wallpaper);
        ConsoleManager::set_wallpaper_changed_message(std::format("* Wallpaper set to: '{}'", wallpaper));
        ConsoleManager::signal_update();

    } else {
        valid_command = false;
    }

    if (valid_command) {
        ConsoleManager::clear_error_message();
    }
    
    return valid_command;
}





// =================================================
// ========== WALLPAPER CHANGER SERVICE ============
// =================================================


void service_wallpaper_changer() {
    while (true) {
        if (state.load() == STOPPED) {
            return;
        } else if (state.load() == PAUSED) {
            pause_semaphore.acquire();
            continue;
        } 
        if (WallpaperManager::empty()) {
            empty_semaphore.acquire();
            continue;
        } else if (not timer.is_done()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        timer.init();
        std::string wallpaper = WallpaperChanger::set_next_wallpaper();

        ConsoleManager::set_wallpaper_changed_message(std::format("* New wallpaper : '{}'", wallpaper));
        ConsoleManager::signal_update();

    }
}





// ========================================
// ============ MAIN FUNCTION =============
// ========================================

void main_function() {

    // Path to the image file
    ConsoleManager::initialize();
    WallpaperManager::initialize(); 
    WallpaperRandomGetter::initialize();
    WallpaperOrderGetter::initialize();
    WallpaperChanger::initialize();
    Parameters::initialize();

    // Load parameters from file
    Parameters::load("settings.wallpaper");

    std::thread wallpaper_thread(service_wallpaper_changer);


    while (true) {
        ConsoleManager::update();
        std::vector<std::string> input = ConsoleManager::get_input();

        if (input.empty()) {
            continue;
        }
        
        std::string command = input[0];
        if (command == "exit") {
            state.store(STOPPED);
            pause_semaphore.release(); 
            empty_semaphore.release(); 
            break;

        } else {
            if (not action(input)) {
                ConsoleManager::set_error_message(std::format("* Command '{}' not recognized", command));
            }
        }

    }

    Parameters::save("settings.wallpaper");

    wallpaper_thread.join();
}


int main() {

    main_function();
    return 0;
}
