#ifndef TARGET_HPP
#define TARGET_HPP

#include <vector>
#include <filesystem>

struct Target
{
    std::filesystem::path::string_type directory;
    int depth = -1;
    std::vector<std::filesystem::path::string_type> extensions;
};

#endif