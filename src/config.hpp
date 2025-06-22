#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <array>

#include "target.hpp"

inline const std::array<std::array<std::string, 2>, 3> colors =  {{
    /* fg        bg        */
    { "#bbbbbb", "#222222" },
    { "#eeeeee", "#005577" },
    { "#000000", "#00ffff" }
}};

// Sources
inline const std::vector<Target> targets = {
    /* Path                         depth       extensions*/
    {"C:\\Program Files",           1,          {".exe"}},
    {"C:\\Program Files (x86)",     1,          {".exe"}}
};

// Lines
// If non-zero, use vertical line layout
inline const unsigned int lines = 0;

#endif