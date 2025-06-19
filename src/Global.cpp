#include "Global.hpp"
#include "CommandManager.hpp"
#include "WallpaperChanger.hpp"
#include "WallpaperManager.hpp"
#include "Parameter.hpp"
#include "App.hpp"
#include "WallpaperChangerService.hpp"
#include "Utility.hpp"

#include "rmz_format.hpp"

#include <shlobj.h>

// std::counting_semaphore<0> pause_semaphore(0);
// std::counting_semaphore<0> empty_semaphore(0);
// std::atomic<State> state{RUNNING};
std::string save_file = "settings.wallpaper";
bool auto_save = true;

void add_wallpaper_folder(const std::string& folder) {
    WallpaperManager::add_folder(folder);
    WallpaperChanger::refresh();
    WallpaperChangerService::notify_added_wallpaper();
    auto [wallpapers, count] = count_wallpapers_message(folder);
    App::set_info_message(rmz::format("Added folder: '{}' containing {} wallpapers:\n{}", folder, count, wallpapers));
}

bool action(const std::string& input) {
    bool valid_command = true;
    
    if (CommandManager::Order::parse(input)) {
        // CommandManager::Order::run();
        WallpaperChanger::set_change_order_sequential();

    } else if (CommandManager::Random::parse(input)) {
        // CommandManager::Random::run();
        WallpaperChanger::set_change_order_random();
        
    } else if (auto result = CommandManager::Duration::parse(input)) {
        WallpaperChanger::set_duration(result.get());

    } else if (CommandManager::Pause::parse(input)) {
        // CommandManager::Pause::run();
        WallpaperChangerService::pause();

    } else if (auto result = CommandManager::Add::parse(input)) {
        // CommandManager::Add::run(result.get());
        WallpaperManager::add_folder(result.get());
        WallpaperChanger::refresh();
        WallpaperChangerService::notify_added_wallpaper();

    } else if (CommandManager::Resume::parse(input)) {
        // CommandManager::Resume::run();
        WallpaperChangerService::resume();

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

void pause_wallpaper_changer() { WallpaperChangerService::pause(); }
void resume_wallpaper_changer() { WallpaperChangerService::resume(); }

std::string OpenModernFolderPicker() {
    std::string result;
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr)) {
        IFileDialog* pFileDialog = nullptr;
        hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog));

        if (SUCCEEDED(hr)) {
            DWORD options;
            pFileDialog->GetOptions(&options);
            pFileDialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);

            hr = pFileDialog->Show(nullptr);
            if (SUCCEEDED(hr)) {
                IShellItem* pItem = nullptr;
                if (SUCCEEDED(pFileDialog->GetResult(&pItem))) {
                    PWSTR widePath = nullptr;
                    if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &widePath))) {
                        result = {widePath, widePath + wcslen(widePath)};
                        CoTaskMemFree(widePath);
                    }
                    pItem->Release();
                }
            }
            pFileDialog->Release();
        }
        CoUninitialize();
    }
    return result;
}