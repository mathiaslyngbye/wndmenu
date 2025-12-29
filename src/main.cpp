#include <vector>
#include <string>

#include "gui.hpp"
#include "config.hpp"
#include "scan.hpp"
#include "tree.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
    // Index files
    FileIndex index = scanTargets(targets); /* ~85ms */

    // Captilization indifferent sort
    std::sort(
        index.entries.begin(),
        index.entries.end(),
        [&](const FileEntry& a, const FileEntry& b) {
            return (compare(view(index, a.name), view(index, b.name)) < 0);
        }
    );

    auto find = [&](const std::wstring& prefix)
    {
        auto matches = search(index, prefix);

        std::vector<Suggestion> out;
        out.reserve(matches.size());

        for (const FileEntry* entry : matches)
        {
            std::wstring_view full = view(index, entry->path);

            std::filesystem::path p(full);

            std::wstring directory = p.parent_path().native();
            std::wstring name(view(index, entry->name));

            out.push_back({
                std::move(directory),
                std::move(name)
            });
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
