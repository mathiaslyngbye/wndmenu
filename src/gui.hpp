#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <functional>

using SuggestionProvider = std::function<std::vector<std::wstring>(const std::wstring&)>;
using SelectionHandler   = std::function<void(const std::wstring&)>;

class PrefixMenuBar {
public:
    PrefixMenuBar(SuggestionProvider provider,
                  SelectionHandler selector,
                  int height = 30,
                  int maxSuggestions = 5)
        : provider(provider),
          onSelect(selector),
          barHeight(height),
          lineHeight(height),
          maxSuggestions(maxSuggestions),
          hInstance(GetModuleHandle(nullptr)) {}

    void run() {
        registerClass();
        createWindow();
        messageLoop();
    }

    void runAsync() {
        CreateThread(nullptr, 0, [](LPVOID param) -> DWORD {
            static_cast<PrefixMenuBar*>(param)->run();
            return 0;
        }, this, 0, nullptr);
    }

private:
    SuggestionProvider provider;
    SelectionHandler onSelect;
    int barHeight;
    int maxSuggestions;
    int lineHeight = 20;
    HWND _hwnd;
    HINSTANCE hInstance;
    std::wstring _input;
    std::vector<std::wstring> _suggestions;
    int _selectedIndex = 0;

    static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        PrefixMenuBar* self = nullptr;
        if (msg == WM_NCCREATE) {
            self = static_cast<PrefixMenuBar*>(reinterpret_cast<CREATESTRUCT*>(lp)->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        } else {
            self = reinterpret_cast<PrefixMenuBar*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }
        return self ? self->WndProc(hwnd, msg, wp, lp) : DefWindowProc(hwnd, msg, wp, lp);
    }

    void registerClass() {
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
        int height = barHeight + maxSuggestions * lineHeight;
        int x = (GetSystemMetrics(SM_CXSCREEN)/2)-(width/2);
        int y = (GetSystemMetrics(SM_CYSCREEN)/2)-(height/2);

        _hwnd = CreateWindowEx(
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
        SetFocus(_hwnd);
    }

    void messageLoop() {
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void updateSuggestions()
    {
        _suggestions = provider(_input);
        if (_suggestions.size() > maxSuggestions)
            _suggestions.resize(maxSuggestions);
        _selectedIndex = 0;
        InvalidateRect(_hwnd, nullptr, TRUE);
    }

    void draw()
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(_hwnd, &ps);
        RECT rect;
        GetClientRect(_hwnd, &rect);
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        SetBkMode(hdc, TRANSPARENT);
        RECT inputRect = {0, 0, rect.right, barHeight};
        DrawTextW(hdc, _input.c_str(), -1, &inputRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        for (size_t i = 0; i < _suggestions.size(); ++i) {
            RECT line = {0, barHeight + int(i * lineHeight), rect.right, barHeight + int((i+1) * lineHeight)};
            if ((int)i == _selectedIndex) {
                HBRUSH highlight = CreateSolidBrush(RGB(200, 200, 255));
                FillRect(hdc, &line, highlight);
                DeleteObject(highlight);
            }
            DrawTextW(hdc, _suggestions[i].c_str(), -1, &line, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }
        EndPaint(_hwnd, &ps);
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
                else if (wp == VK_BACK && !_input.empty())
                {
                    _input.pop_back();
                    updateSuggestions();
                } 
                else if (wp >= 32 && wp <= 126)
                {
                    _input.push_back(static_cast<wchar_t>(wp));
                    updateSuggestions();
                }

                return 0;

            // List navigation
            case WM_KEYDOWN:
                if ((wp == VK_DOWN) || (wp == VK_TAB && !(GetKeyState(VK_SHIFT) & 0x8000)))
                {
                    _selectedIndex = (_selectedIndex + 1) % _suggestions.size();
                    InvalidateRect(hwnd, nullptr, TRUE);
                } 
                else if ((wp == VK_UP) || (wp == VK_TAB && (GetKeyState(VK_SHIFT) & 0x8000)))
                {
                    _selectedIndex = (_selectedIndex + _suggestions.size() - 1) % _suggestions.size();
                    InvalidateRect(hwnd, nullptr, TRUE);
                } 
                else if (wp == VK_RETURN)
                {
                    if (!_suggestions.empty())
                        onSelect(_suggestions[_selectedIndex]);
                    else
                        onSelect(_input);
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
