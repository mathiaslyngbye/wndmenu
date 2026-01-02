#include <vector>
#include <string>

#include "control.hpp"
#include "index.hpp"
#include "app.hpp"
#include "gui.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
    Index index = scan();

    App app{};
    app.instance = hInstance;
    app.index = &index;

    return run(app);
}
