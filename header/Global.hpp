
#pragma once

#include <atomic>
#include <string>
#include <semaphore>
#include <vector>


extern std::string save_file;
extern bool auto_save;

void add_folder(const std::string& folder);
void add_wallpapers(const std::vector<std::string>& wallpapers);

bool action(const std::string& input);


void pause_wallpaper_changer();
void resume_wallpaper_changer();

std::string OpenModernFolderPicker();