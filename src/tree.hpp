#ifndef TREE_HPP
#define TREE_HPP

#include <vector>

struct ScanEntry
{
    std::string name;
    uint32_t index;
};

struct ScanResult
{
    std::vector<std::string> directories;
    std::vector<ScanEntry> entries;
};

#endif