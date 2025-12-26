#include <vector>
#include <string>

#include "gui.hpp"
#include "config.hpp"
#include "scan.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
    ScanResult result = scanTargets(targets);
    sort(result.entries);
    
    auto find = [&](const std::wstring& prefix)
    {
        std::string narrow(prefix.begin(), prefix.end());
        auto matches = search(result.entries, narrow, 10);
        
        // For now, copy all matches
        std::vector<Suggestion> out;
        for (const ScanEntry* entry : matches)
        {
            std::wstring directory(result.directories[entry->index].begin(), result.directories[entry->index].end());
            std::wstring name(entry->name.begin(), entry->name.end());
            out.push_back({directory, name});
        }

        return out;
    };
    
    auto launch = [&](const Suggestion& choice)
    {
        // Get command
        std::filesystem::path full = std::filesystem::path(choice[0]) / choice[1];
        std::wstring cmd = L"\"" + full.wstring() + L"\"";

        // Get process info
        STARTUPINFOW si = {};
        PROCESS_INFORMATION pi = {};
        si.cb = sizeof(si);

        CreateProcessW(
            NULL,               // Application name (NULL = use command line)
            cmd.data(),         // Command line (mutable!)
            NULL,               // Process security attributes
            NULL,               // Thread security attributes
            FALSE,              // Inherit handles
            CREATE_NEW_CONSOLE, // Flags: new console, detached
            NULL,               // Use parent's environment
            choice[0].c_str(),  // Use parent's current directory
            &si,                // Startup info
            &pi                 // Process info
        );

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    };

    PrefixMenuBar bar(find, launch);
    bar.run();

    return 0;
}
