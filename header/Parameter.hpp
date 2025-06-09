#pragma once

#include <string>
#include <vector>
#include <fstream>


namespace Parameters {
    void initialize();
    void load(const std::string& name);
    void save(const std::string& name);

    void write_line(std::ofstream& file, const std::string& line);
    std::vector<std::string> read(const std::string& name);
};