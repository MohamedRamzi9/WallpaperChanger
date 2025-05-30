
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




// ======================================
// ============ DECLARATIONS ============
// ======================================

bool action(const std::string& input);




struct WallpaperChanger {
    static enum change_order { ORDER, RANDOM } change_order;
    using duration_type = rmz::seconds; static rmz::timer timer; 

    static void initialize() {
        change_order = ORDER;
        timer.set_duration(duration_type(10)); 
        timer.init(); 
    }
    static void set_change_order(enum change_order new_order) {
        change_order = new_order;
    }
    static void set_wallpaper(const std::string& path) {
        SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (PVOID)path.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
    }
    static std::string set_next_wallpaper();
    static std::string set_previous_wallpaper();

    static bool is_change_order_random() {
        return change_order == RANDOM;
    }
    static bool is_change_order_order() {
        return change_order == ORDER;
    }
    static duration_type get_duration() {
        return timer.get_duration();
    }
    static bool is_timer_done() {
        return timer.is_done();
    }
    static void set_duration(duration_type duration) {
        timer.set_duration(duration);
        timer.init();
    }
    static void init_timer() {
        timer.init();
    }
};
enum WallpaperChanger::change_order WallpaperChanger::change_order;
rmz::timer WallpaperChanger::timer; 




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

std::string join_string(const std::vector<std::string>& vec, const std::string& delimiter) {
    if (vec.empty()) return "";
    std::string result = vec[0];
    for (size_t i = 1; i < vec.size(); ++i) {
        result += delimiter + vec[i];
    }
    return result;
}

rmz::type::nullable<WallpaperChanger::duration_type> parse_duration(const std::string& input) {
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




// ======================================
// ============ DEFINITIONS =============
// ======================================

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
    static std::vector<std::string>& get_folders() {
        return folders;
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




struct CommandManager {
    static std::vector<std::string> strings;
    static std::vector<std::string> descriptions;

    static void initialize() {
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
    static std::vector<std::string> get_strings() {
        return strings;
    }
    static std::vector<std::string> get_descriptions() {
        return descriptions;
    }
    
    struct Duration {
        static std::string get_string(const WallpaperChanger::duration_type& duration) { return std::format("{}s", duration.count()); }
        static std::string get_string() { return "<int>s/m"; }
        static std::string get_description() { return "Set the wallpaper change interval (e.g., '10s' for 10 seconds, '5m' for 5 minutes)."; }
        static rmz::type::nullable<WallpaperChanger::duration_type> parse(const std::string& input) {
            return parse_duration(input);
        }
    };
    struct Add {
        static std::string get_string(const std::vector<std::string>& folders) { return "add " + join_string(folders, " "); }
        static std::string get_string() { return "add <folder>"; }
        static std::string get_description() { return "Add a folder to the list of wallpaper folders and load its wallpapers."; }
        static rmz::type::nullable<std::string> parse(const std::string& input) {
            auto parts = split_string(input, ' ');
            if (parts.size() < 2) return {};
            if (parts[0] != "add") return {};
            // parts.erase(parts.begin()); 
            return parts[1];
        }
    };
    struct Order {
        static std::string get_string() { return "order"; }
        static std::string get_description() { return "Set wallpaper change order to sequential."; }
        static bool parse(const std::string& input) { return input == "order"; } 
    };
    struct Random {
        static std::string get_string() { return "random"; }
        static std::string get_description() { return "Set wallpaper change order to random."; }
        static bool parse(const std::string& input) { return input == "random"; }
    };
    struct Pause {
        static std::string get_string() { return "pause"; }
        static std::string get_description() { return "Pause the automatic wallpaper changer."; }
        static bool parse(const std::string& input) { return input == "pause"; }
    };
    struct Resume {
        static std::string get_string() { return "resume"; }
        static std::string get_description() { return "Resume the automatic wallpaper changer."; }
        static bool parse(const std::string& input) { return input == "resume"; }
    };
    struct Next {
        static std::string get_string() { return "next"; }
        static std::string get_description() { return "Set the next wallpaper in the current order."; }
        static bool parse(const std::string& input) { return input == "next"; }
    };
    struct Previous {
        static std::string get_string() { return "previous"; }
        static std::string get_description() { return "Set the previous wallpaper in the current order."; }
        static bool parse(const std::string& input) { return input == "previous"; }
    };
    struct Set {
        static std::string get_string() { return "set <path>"; }
        static std::string get_description() { return "Set the wallpaper to a specific image file."; }
        static rmz::type::nullable<std::string> parse(const std::string& input) {
            auto parts = split_string(input, ' ');
            if (parts.size() != 2) return {};
            if (parts[0] != "set") return {};
            return parts[1];
        }
    };
    struct Help {
        static std::string get_string() { return "help"; }
        static std::string get_description() { return "Show this help message."; }
        static bool parse(const std::string& input) { return input == "help"; }
    };
    struct Clear {
        static std::string get_string() { return "clear"; }
        static std::string get_description() { return "Clear the help and error messages."; }
        static bool parse(const std::string& input) { return input == "clear"; }
    };
    struct Parameters {
        static std::string get_string() { return "parameters"; }
        static std::string get_description() { return "Show the current parameters."; }
        static bool parse(const std::string& input) { return input == "parameters"; }
    };
    struct Exit {
        static std::string get_string() { return "exit"; }
        static std::string get_description() { return "Exit the program and stop the automatic wallpaper changer."; }
        static bool parse(const std::string& input) { return input == "exit"; }
    };
};
std::vector<std::string> CommandManager::strings;
std::vector<std::string> CommandManager::descriptions;




struct ConsoleManager {

    static std::atomic<bool> signaled;
    static std::string wallpaper_changed_message;
    static std::string error_message;
    static bool show_help_flag;
    static bool show_parameters_flag;

    static void initialize() { rmz::enable_ansi(); }

    
    static void show_help() {
        show_help_flag = true;
    }
    static void clear_help() {
        show_help_flag = false;
    }

    static void show_parameters() {
        show_parameters_flag = true;
    }
    static void clear_parameters() {
        show_parameters_flag = false;
    }

    static void set_wallpaper_changed_message(const std::string& message) {
        wallpaper_changed_message = message;
    }
    static void set_error_message(const std::string& message) {
        error_message = message;
    }
    static void add_error_message(const std::string& message) {
        error_message += message + '\n'; 
    }

    static void signal_update() {
        signaled.store(true);
        rmz::insert_enter();
    }
    static void clear_signal() { signaled.store(false); }
    static void clear_error_message() { error_message.clear(); }
    static bool is_signaled() { return signaled.load(); }
    
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

        if (show_parameters_flag) {
            print_parameters();
            rmz::println();
        }

        if (show_help_flag) {
            print_help();
            rmz::println();
        } else {
            print_commands();
            rmz::println();
        }
            
    }

    static std::string get_input() {
        std::string raw_input;
        std::getline(std::cin, raw_input);
        while (ConsoleManager::is_signaled()) {
            ConsoleManager::clear_signal();
            ConsoleManager::update();
            rmz::insert_input(raw_input);
            std::getline(std::cin, raw_input);
        }
        return raw_input;
    }
    
private:    
    static void print_commands() {
        rmz::println("* Commands: {}", join_string(CommandManager::get_strings(), ", "));
    }
    static void print_help() {
        rmz::println("* Available commands:");
        for (auto [command, description] : std::views::zip(CommandManager::get_strings(), CommandManager::get_descriptions())) {
            rmz::println("  - '{}': {}", command, description);
        }
    }
    static void print_parameters() {
        rmz::println("* Current parameters:");
        rmz::println("  - Duration: {}s", WallpaperChanger::get_duration().count());
        rmz::println("  - Change order: {}", WallpaperChanger::is_change_order_order() ? "Order" : "Random");
        rmz::println("  - Automatic wallpaper changer state: {}", state.load() == RUNNING ? "Running" : (state.load() == PAUSED ? "Paused" : "Stopped"));
        rmz::println("  - Folders: {}", join_string(WallpaperManager::get_folders(), ", "));
    }
};
std::atomic<bool> ConsoleManager::signaled(false);
std::string ConsoleManager::wallpaper_changed_message;
std::string ConsoleManager::error_message;
bool ConsoleManager::show_help_flag = false;
bool ConsoleManager::show_parameters_flag = false;




struct Parameters {
    
    static void initialize() {}

    static void load(const std::string& name) {
        auto parameters = read(name);
        for (auto& parameter : parameters) {
            if (not action(parameter)) {
                ConsoleManager::add_error_message(std::format("* Command '{}' not recognized in file '{}'", parameter, name));
            }
        }
    }

    static void save(const std::string& name) {
        std::ofstream file(name, std::ios::trunc);

        write_line(file, CommandManager::Duration::get_string(WallpaperChanger::get_duration()));
        write_line(file, CommandManager::Add::get_string(WallpaperManager::get_folders()));
        write_line(file, WallpaperChanger::is_change_order_order() ? CommandManager::Order::get_string() : CommandManager::Random::get_string());

        file.close();
    }



private:
    static void write_line(std::ofstream& file, const std::string& line) {
        file << line << '\n';
    }
    static std::vector<std::string> read(const std::string& name) {
        std::vector<std::string> result;
        std::ifstream file(name);

        std::string line;
        while (std::getline(file, line)) {
            result.push_back(line);
        }
        return result;
    }

};




bool action(const std::string& input) {
    bool valid_command = true;
    
    if (CommandManager::Order::parse(input)) {
        WallpaperChanger::set_change_order(WallpaperChanger::ORDER);

    } else if (CommandManager::Random::parse(input)) {
        WallpaperChanger::set_change_order(WallpaperChanger::RANDOM);
        
    } else if (auto result = CommandManager::Duration::parse(input)) {
        WallpaperChanger::set_duration(result.get());

    } else if (CommandManager::Pause::parse(input)) {
        state.store(PAUSED);

    } else if (auto result = CommandManager::Add::parse(input)) {
        WallpaperManager::add_folder(result.get());
        WallpaperRandomGetter::refresh();
        WallpaperOrderGetter::refresh();
        empty_semaphore.release();

    } else if (CommandManager::Resume::parse(input)) {
        state.store(RUNNING);
        pause_semaphore.release();

    } else if (CommandManager::Next::parse(input)) {
        WallpaperChanger::set_next_wallpaper();

    } else if (CommandManager::Previous::parse(input)) {
        WallpaperChanger::set_previous_wallpaper();

    } else if (auto result = CommandManager::Set::parse(input)) {
        WallpaperChanger::set_wallpaper(result.get());
        ConsoleManager::set_wallpaper_changed_message(std::format("* Wallpaper set to: '{}'", result.get()));
        ConsoleManager::signal_update();

    } else if (CommandManager::Help::parse(input)) {
        ConsoleManager::show_help();

    } else if (CommandManager::Parameters::parse(input)) {
        ConsoleManager::show_parameters();

    } else if (CommandManager::Clear::parse(input)) {
        ConsoleManager::clear_error_message();
        ConsoleManager::clear_parameters();

    } else {
        valid_command = false;
    }

    if (valid_command) {
        ConsoleManager::clear_error_message();
    }
    
    return valid_command;
}





std::string WallpaperChanger::set_next_wallpaper() {
    std::string wallpaper;
    if (change_order == ORDER) {
        wallpaper = WallpaperOrderGetter::get_next_wallpaper();
    } else {
        wallpaper = WallpaperRandomGetter::get_next_wallpaper();
    }
    set_wallpaper(wallpaper);
    return wallpaper;
}
std::string WallpaperChanger::set_previous_wallpaper() {
    std::string wallpaper;
    if (change_order == ORDER) {
        wallpaper = WallpaperOrderGetter::get_previous_wallpaper();
    } else {
        wallpaper = WallpaperRandomGetter::get_previous_wallpaper();
    }
    set_wallpaper(wallpaper);
    return wallpaper;
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
        } else if (not WallpaperChanger::is_timer_done()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        WallpaperChanger::init_timer();
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
    CommandManager::initialize();

    // Load parameters from file
    Parameters::load("settings.wallpaper");

    std::thread wallpaper_thread(service_wallpaper_changer);


    while (true) {
        ConsoleManager::update();
        auto input = ConsoleManager::get_input();

        if (input.empty()) {
            continue;
        }

        if (CommandManager::Exit::parse(input)) {
            state.store(STOPPED);
            pause_semaphore.release(); 
            empty_semaphore.release(); 
            break;

        } else {
            if (not action(input)) {
                ConsoleManager::set_error_message(std::format("* Command '{}' not recognized", input));
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
