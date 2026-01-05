#include "control.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <filesystem>

#include "index.hpp"
#include "compare.hpp"

static std::wstring stem(std::wstring_view path)
{
    size_t slash = path.find_last_of(L"\\/");
    if (slash != std::wstring_view::npos)
        slash += 1;
    else
        slash = 0;

    std::wstring_view name = path.substr(slash);

    size_t dot = name.find_last_of(L'.');
    if (dot != std::wstring_view::npos)
        name = name.substr(0, dot);

    return std::wstring(name);
}

static bool addEntry(Index& index, std::unordered_set<std::wstring>& seen, std::wstring_view name, std::wstring_view path)
{
    if (name.empty() || path.empty())
        return false;

    auto [it, inserted] = seen.emplace(name);
    if (!inserted)
        return false;

    Entry entry{};
    entry.name = intern(index.pool, name);
    entry.path = intern(index.pool, path);
    index.entries.push_back(entry);

    return true;
}

static void addDirectory(Index& index, std::unordered_set<std::wstring>& seen, const std::filesystem::path& root)
{
    std::error_code errorCode;
    std::filesystem::recursive_directory_iterator directories(
        root,
        std::filesystem::directory_options::skip_permission_denied,
        errorCode
    );

    if (errorCode)
        return;

    for (const auto& entry : directories)
    {
        if (entry.is_directory(errorCode))
        {
            errorCode.clear();
            continue;
        }

        if (!entry.is_regular_file(errorCode))
        {
            errorCode.clear();
            continue;
        }

        const auto& path = entry.path();
        std::wstring_view filename(path.filename().native());

        if (!(endsWith(filename, L".lnk") || endsWith(filename, L".url") || endsWith(filename, L".appref-ms")))
            continue;

        std::wstring display = stem(filename);
        std::wstring command = path.native();
        addEntry(index, seen, display, command);
    }
}

static void addStartMenu(Index& index, std::unordered_set<std::wstring>& seen)
{
    const KNOWNFOLDERID folders[] = { FOLDERID_Programs, FOLDERID_CommonPrograms };

    for (const KNOWNFOLDERID& id : folders)
    {
        PWSTR pointer = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(id, KF_FLAG_DEFAULT, nullptr, &pointer)) && pointer)
        {
            std::filesystem::path root(pointer);
            CoTaskMemFree(pointer);

            addDirectory(index, seen, root);
        }
    }
}

Index scan()
{
    Index index;
    index.entries.reserve(16384);
    index.pool.reserve(1 << 21);

    std::unordered_set<std::wstring> seen;
    seen.reserve(32768);

    addStartMenu(index, seen);

    return index;
}

void search(const Index& index, std::wstring_view query, std::vector<uint32_t>& out)
{
    out.clear();
    std::vector<uint8_t> found(index.entries.size(), 0);

    // Prefix matching
    for (uint32_t i = 0; i < index.entries.size(); i++)
    {
        const Entry& entry = index.entries[i];
        const std::wstring_view name = view(index, entry.name);

        if (startsWith(name, query))
        {
            out.push_back(i);
            found[i] = 1;
        }
    }

    // Substring matching
    for (uint32_t i = 0; i < index.entries.size(); i++)
    {
        if (found[i])
            continue;

        const Entry& entry = index.entries[i];
        const std::wstring_view name = view(index, entry.name);

        if (contains(name, query))
            out.push_back(i);
    }
}

void launch(const Index& index, uint32_t id)
{
    if (id >= index.entries.size())
        return;

    const Entry& e = index.entries[id];

    ShellExecuteW(
        nullptr,
        L"open",
        c_str(index, e.path),
        nullptr,
        nullptr,
        SW_SHOWNORMAL
    );
}

std::wstring_view display(const Index& index, uint32_t id)
{
    if (id >= index.entries.size())
        return L"";

    return view(index, index.entries[id].name);
}
