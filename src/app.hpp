#ifndef APP_HPP
#define APP_HPP

#include <windows.h>
#include <string>
#include <vector>
#include <cstdint>

struct Index;

struct App
{
    /* index */
    const Index* index = nullptr;

    /* gui */
    HINSTANCE instance{};
    HWND window{};

    /* layout */
    int lines       = 10;
    int lineWidth   = 500;
    int lineHeight  = 19;
    int indent      = 6;

    /* io */
    std::wstring query;
    std::vector<uint32_t> results;
    int selected = 0;

    int page() const
    {
        return (lines > 0) ? (selected / lines) * lines : 0;
    }
};

#endif