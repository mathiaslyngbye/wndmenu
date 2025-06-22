#ifndef TARGET_HPP
#define TARGET_HPP

#include <string>
#include <vector>

struct Target
{
    std::string directory;
    int depth = -1;
    std::vector<std::string> extensions;
};

#endif