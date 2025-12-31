#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <array>
#include <functional>

#include "config.hpp"

using Suggestion         = std::array<std::wstring, 2>; // path, binary
using SuggestionProvider = std::function<std::vector<Suggestion>(const std::wstring&)>;
using SelectionHandler   = std::function<void(const Suggestion&)>;

class PrefixMenuBar
{
public:
    PrefixMenuBar(SuggestionProvider provider, SelectionHandler selector)
        : provider(provider),
          onSelect(selector),
          hInstance(GetModuleHandle(nullptr))
    {}

    void run()
    {
        registerClass();
        createWindow();
        messageLoop();
    }

private:

    SuggestionProvider provider;
    SelectionHandler onSelect;
    int lines = 10;
    int lineHeight = 19;
    int lineIndent = 6;
    HWND hwnd;
    HINSTANCE hInstance;
    std::wstring input;
    std::vector<std::array<std::wstring, 2>> suggestions;
    int selectedIndex = 0;

    static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
    {
        PrefixMenuBar* self = nullptr;
        if (msg == WM_NCCREATE)
        {
            self = static_cast<PrefixMenuBar*>(reinterpret_cast<CREATESTRUCT*>(lp)->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        }
        else
            self = reinterpret_cast<PrefixMenuBar*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        return self ? self->WndProc(hwnd, msg, wp, lp) : DefWindowProc(hwnd, msg, wp, lp);
    }

    void registerClass()
    {
        WNDCLASS wc = {};
        wc.lpfnWndProc = StaticWndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = L"PrefixMenuBarClass";
        wc.hCursor = LoadCursor(nullptr, IDC_IBEAM);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        RegisterClass(&wc);
    }

    void createWindow()
    {
        int width = 500;
        int height = lineHeight + (lines * lineHeight);
        int x = (GetSystemMetrics(SM_CXSCREEN)/2)-(width/2);
        int y = (GetSystemMetrics(SM_CYSCREEN)/2)-(height/2);

        hwnd = CreateWindowEx(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
            L"PrefixMenuBarClass",
            nullptr,
            WS_POPUP | WS_VISIBLE,
            x,
            y,
            width,
            height,
            nullptr, nullptr,
            hInstance,
            this
        );
        SetFocus(hwnd);
    }

    void messageLoop()
    {
        MSG msg;
        updateSuggestions();
        while (GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void updateSuggestions()
    {
        suggestions = provider(input);
        selectedIndex = 0;
        InvalidateRect(hwnd, nullptr, TRUE);
    }

    void draw()
    {
        // Initialize
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Draw background
        RECT backgroundRect;
        GetClientRect(hwnd, &backgroundRect);
        HBRUSH backgroundBrush = CreateSolidBrush(colors[0][1]);
        FillRect(hdc, &backgroundRect, backgroundBrush);
        DeleteObject(backgroundBrush);

        // Draw input text
        SetBkMode(hdc, TRANSPARENT);
        RECT inputRect = {lineIndent, 0, backgroundRect.right, lineHeight};
        SetTextColor(hdc, colors[0][0]);
        DrawTextW(hdc, input.c_str(), -1, &inputRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        // Draw suggestions
        for (size_t i = 0; i < lines; i++)
        {
            RECT lineRect = {0,             (lineHeight + int(i * lineHeight)), backgroundRect.right,   (lineHeight + int((i+1) * lineHeight))};
            RECT textRect = {lineIndent,    (lineHeight + int(i * lineHeight)), backgroundRect.right,   (lineHeight + int((i+1) * lineHeight))};

            // Draw extra rectangle on highlight
            int suggestionIndex = (selectedIndex / lines)*lines + i;
            if ((int)suggestionIndex == selectedIndex)
            {
                HBRUSH lineBrush = CreateSolidBrush(colors[1][1]);
                FillRect(hdc, &lineRect, lineBrush);
                DeleteObject(lineBrush);
            }

            // Draw text regardless
            if (suggestionIndex < suggestions.size())
            {
                SetTextColor(hdc, (i == (selectedIndex % lines)) ? colors[1][0] : colors[0][0]);
                DrawTextW(hdc, suggestions[suggestionIndex][1].c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            }
        }

        // End
        EndPaint(hwnd, &ps);
    }

    LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
    {
        switch (msg)
        {
            // Input handling
            case WM_CHAR:
                if (wp == VK_ESCAPE)
                {
                    DestroyWindow(hwnd);
                }
                else if (wp == VK_BACK && !input.empty())
                {
                    input.pop_back();
                    updateSuggestions();
                }
                else if (wp >= 32 && wp <= 126)
                {
                    input.push_back(static_cast<wchar_t>(wp));
                    updateSuggestions();
                }

                return 0;

            // List navigation
            case WM_KEYDOWN:
                if (wp == VK_DOWN)
                {
                    selectedIndex = (selectedIndex < suggestions.size()-1) ? (selectedIndex + 1) : (suggestions.size() - 1);
                    InvalidateRect(hwnd, nullptr, TRUE);
                }
                else if (wp == VK_UP)
                {
                    selectedIndex = (selectedIndex > 0) ? (selectedIndex - 1) : 0;
                    InvalidateRect(hwnd, nullptr, TRUE);
                }
                else if (wp == VK_TAB)
                {
                    input = suggestions[0][1].c_str();
                    selectedIndex = 0;
                    InvalidateRect(hwnd, nullptr, TRUE);
                    updateSuggestions();
                }
                else if (wp == VK_RETURN)
                {
                    if (!suggestions.empty())
                        onSelect(suggestions[selectedIndex]);
                    //else // TODO Handle default input
                    //    onSelect(input);
                    DestroyWindow(hwnd);
                }
                return 0;

            case WM_PAINT:
                draw();
                return 0;

            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;

            case WM_KILLFOCUS:
                DestroyWindow(hwnd);
                return 0;

            default:
                return DefWindowProc(hwnd, msg, wp, lp);
        }
    }
};
