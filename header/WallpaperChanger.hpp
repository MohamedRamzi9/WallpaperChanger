#pragma once
#include <string>
#include "rmz_time.hpp"

namespace WallpaperChanger {
    extern enum change_order { ORDER, RANDOM } change_order;
    using duration_type = rmz::seconds; extern rmz::timer timer;

    void initialize();
    void set_change_order(enum change_order new_order);
    void set_wallpaper(const std::string& path);
    std::string set_next_wallpaper();
    std::string set_previous_wallpaper();

    bool is_change_order_random();
    bool is_change_order_order();
    duration_type get_duration();
    bool is_timer_done();
    void set_duration(duration_type duration);
    void init_timer();
};