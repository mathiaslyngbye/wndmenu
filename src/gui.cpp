#include "gui.hpp"

#include <windows.h>
#include <algorithm>

#include "app.hpp"
#include "control.hpp"
#include "config.hpp"

static void refresh(App& app)
{
    app.selected = 0;
    app.results.clear();
    search(*app.index, app.query, app.results);

    InvalidateRect(app.window, nullptr, TRUE);
}

static void activate(App& app)
{
    if (!app.results.empty())
        launch(*app.index, app.results[app.selected]);

    DestroyWindow(app.window);
}

static void paint(const App& app)
{
    PAINTSTRUCT paint{};
    HDC hdc = BeginPaint(app.window, &paint);

    RECT client{};
    GetClientRect(app.window, &client);

    /* Background */
    {
        HBRUSH bg = CreateSolidBrush(colors[0][1]);
        FillRect(hdc, &client, bg);
        DeleteObject(bg);
    }

    SetBkMode(hdc, TRANSPARENT);

    /* Input */
    {
        RECT textRect{ app.indent, 0, client.right, app.lineHeight };
        SetTextColor(hdc, colors[0][0]);
        DrawTextW(hdc, app.query.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }

    /* Suggestions */
    const int page = app.page();

    for (int row = 0; row < app.lines; row++)
    {
        const int y0 = app.lineHeight + (row * app.lineHeight);
        const int y1 = y0 + app.lineHeight;

        RECT lineRect{ 0, y0, client.right, y1 };
        RECT textRect{ app.indent, y0, client.right, y1 };

        const int source = (page + row);
        const bool selected = (source == app.selected);

        if (selected)
        {
            HBRUSH hl = CreateSolidBrush(colors[1][1]);
            FillRect(hdc, &lineRect, hl);
            DeleteObject(hl);
        }

        if ((source >= 0) && (source < (int)app.results.size()))
        {
            uint32_t id = app.results[source];
            std::wstring_view text = display(*app.index, id);

            SetTextColor(hdc, selected ? colors[1][0] : colors[0][0]);
            DrawTextW(hdc, text.data(), (int)text.size(), &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }
    }

    EndPaint(app.window, &paint);
}

static void onCharacter(App& app, wchar_t character)
{
    /* Backspace */
    if (character == L'\b')
    {
        if (!app.query.empty())
        {
            app.query.pop_back();
            refresh(app);
        }
        return;
    }

    /* Letters and numbers */
    if ((character >= 32) && (character <= 126))
    {
        app.query.push_back(character);
        refresh(app);
    }
}

static void onKey(App& app, WPARAM key)
{
    switch (key)
    {
        case VK_ESCAPE:
            DestroyWindow(app.window);
            break;

        case VK_DOWN:
            if (!app.results.empty())
            {
                app.selected += (app.selected < app.results.size() - 1) ? 1 : 0;
                InvalidateRect(app.window, nullptr, TRUE);
            }
            break;

        case VK_UP:
            if (!app.results.empty())
            {
                app.selected -= (app.selected > 0) ? 1 : 0;
                InvalidateRect(app.window, nullptr, TRUE);
            }
            break;

        case VK_TAB:
            if (!app.results.empty())
            {
                int pick = 0; /* TODO: Figure out tab complete logic */
                std::wstring_view suggestion = display(*app.index, app.results[pick]);
                app.query.assign(suggestion.begin(), suggestion.end());
                app.selected = 0;
                refresh(app);
            }
            break;

        case VK_RETURN:
            activate(app);
            break;
    }
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    App* app = reinterpret_cast<App*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (msg)
    {
        case WM_NCCREATE:
        {
            auto* cs = reinterpret_cast<CREATESTRUCT*>(lp);
            app = static_cast<App*>(cs->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);
            app->window = hwnd;
            return TRUE;
        }

        case WM_CHAR:
            onCharacter(*app, (wchar_t)wp);
            return 0;

        case WM_KEYDOWN:
            onKey(*app, wp);
            return 0;

        case WM_PAINT:
            paint(*app);
            return 0;

        case WM_KILLFOCUS:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, msg, wp, lp);
}

int run(App& app)
{
    /* Initialize */
    WNDCLASS wc{};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = app.instance;
    wc.lpszClassName = L"WndMenuBar";
    wc.hCursor       = LoadCursor(nullptr, IDC_IBEAM);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    const int height = app.lineHeight + app.lines * app.lineHeight;
    const int width  = app.lineWidth;
    const int x = (GetSystemMetrics(SM_CXSCREEN) - app.lineWidth) / 2;
    const int y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

    CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        wc.lpszClassName,
        nullptr,
        WS_POPUP | WS_VISIBLE,
        x, y, app.lineWidth, height,
        nullptr, nullptr,
        app.instance,
        &app
    );

    /* Focus + get default suggestions */
    SetFocus(app.window);
    refresh(app);

    /* Message loop */
    MSG msg{};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
