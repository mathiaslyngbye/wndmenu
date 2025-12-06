#include <vector>
#include <string>

#include "gui.hpp"
#include "config.hpp"
#include "scan.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
    ScanResult result = scanTargets(targets);
    sort(result.entries);
    
    auto search = [&](const std::wstring& prefix) {
        std::vector<std::wstring> out;
        out.push_back(L"TestA");
        out.push_back(L"TestB");
        out.push_back(L"TestC");
        out.push_back(L"TestD");
        return out;
    };

    auto launch = [&](const std::wstring& choice) {
        return;
    };

    PrefixMenuBar bar(search, launch);
    bar.run();

    return 0;
}
