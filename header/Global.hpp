
#pragma once

#include <atomic>
#include <string>
#include <semaphore>

extern std::counting_semaphore<0> pause_semaphore;
extern std::counting_semaphore<0> empty_semaphore;

// State enum and atomic state variable
enum State { RUNNING, PAUSED, STOPPED };
extern std::atomic<State> state;

extern std::string save_file;
extern bool auto_save;

bool action(const std::string& input);
