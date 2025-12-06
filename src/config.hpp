#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <array>

#include "target.hpp"

#include <windows.h>

inline constexpr std::array<std::array<COLORREF, 2>, 3> colors = {{
    /* fg                       bg */
    { RGB(0xbb, 0xbb, 0xbb),    RGB(0x22, 0x22, 0x22) }, /* Normal */
    { RGB(0xee, 0xee, 0xee),    RGB(0x00, 0x55, 0x77) }, /* Selected */
    { RGB(0x00, 0x00, 0x00),    RGB(0x00, 0xff, 0xff) }, /* Out */
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