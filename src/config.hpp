#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "target.hpp"

std::vector<Target> targets = {
    /* Path                         depth       extensions*/
    {"C:\\Program Files",           1,          {".exe"}},
    {"C:\\Program Files (x86)",     1,          {".exe"}}
};

#endif