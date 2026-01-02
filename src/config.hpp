#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <array>
#include <windows.h>

#include "target.hpp"

inline constexpr std::array<std::array<COLORREF, 2>, 3> colors = {{
    /* fg                       bg */
    { RGB(0xbb, 0xbb, 0xbb),    RGB(0x22, 0x22, 0x22) }, /* Normal */
    { RGB(0xee, 0xee, 0xee),    RGB(0x00, 0x55, 0x77) }, /* Selected */
    { RGB(0x00, 0x00, 0x00),    RGB(0x00, 0xff, 0xff) }, /* Out */
}};

/* Sources */
inline const std::vector<Target> targets = {
    /* Path                         depth       extensions */
    {L"C:\\Program Files",           3,         {L".exe"}},
    {L"C:\\Program Files (x86)",     3,         {L".exe"}}
};

/* Layout */
inline const unsigned int lines = 10;

#endif