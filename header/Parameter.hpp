#pragma once

#include <string>
#include <vector>
#include <fstream>


struct Parameters {
    static void initialize();
    static void load(const std::string& name);
    static void save(const std::string& name);

private:
    static void write_line(std::ofstream& file, const std::string& line);
    static std::vector<std::string> read(const std::string& name);
};