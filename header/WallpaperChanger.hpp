#pragma once
#include <string>
#include "rmz_time.hpp"

struct WallpaperChanger {
    static enum change_order { ORDER, RANDOM } change_order;
    using duration_type = rmz::seconds; static rmz::timer timer;

    static void initialize();
    static void set_change_order(enum change_order new_order);
    static void set_wallpaper(const std::string& path);
    static std::string set_next_wallpaper();
    static std::string set_previous_wallpaper();

    static bool is_change_order_random();
    static bool is_change_order_order();
    static duration_type get_duration();
    static bool is_timer_done();
    static void set_duration(duration_type duration);
    static void init_timer();
};