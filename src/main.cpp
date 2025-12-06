#include <vector>
#include <string>

#include "gui.hpp"
#include "config.hpp"
#include "scan.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
    ScanResult result = scanTargets(targets);
    sort(result.entries);
    
    auto search = [&](const std::wstring& prefix)
    {
        std::vector<Suggestion> out;
        out.push_back({L"PATH", L"TestA"});
        out.push_back({L"PATH", L"TestA"});
        out.push_back({L"PATH", L"TestA"});
        out.push_back({L"PATH", L"TestA"});
        return out;
    };
    
    auto launch = [&](const Suggestion& choice)
    {
        ;
    };

    PrefixMenuBar bar(search, launch);
    bar.run();

    return 0;
}
