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

static bool add_entry(Index& index, std::unordered_set<std::wstring>& seen, std::wstring_view name, std::wstring_view path)
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

static void add_directory(Index& index, std::unordered_set<std::wstring>& seen, const std::filesystem::path& root)
{
    std::error_code error;
    std::filesystem::recursive_directory_iterator directories(
        root,
        std::filesystem::directory_options::skip_permission_denied,
        error
    );

    if (error)
        return;

    for (const auto& entry : directories)
    {
        if (entry.is_directory(error))
        {
            error.clear();
            continue;
        }

        if (!entry.is_regular_file(error))
        {
            error.clear();
            continue;
        }

        const auto& path = entry.path();
        std::wstring_view filename(path.filename().native());

        if (!(ends_with(filename, L".lnk") || ends_with(filename, L".url") || ends_with(filename, L".appref-ms")))
            continue;

        std::wstring display = stem(filename);
        std::wstring command = path.native();
        add_entry(index, seen, display, command);
    }
}

static void add_start_menu(Index& index, std::unordered_set<std::wstring>& seen)
{
    const KNOWNFOLDERID folders[] = { FOLDERID_Programs, FOLDERID_CommonPrograms };

    for (const KNOWNFOLDERID& id : folders)
    {
        PWSTR pointer = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(id, KF_FLAG_DEFAULT, nullptr, &pointer)) && pointer)
        {
            std::filesystem::path root(pointer);
            CoTaskMemFree(pointer);

            add_directory(index, seen, root);
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

    add_start_menu(index, seen);

    std::sort(
        index.entries.begin(),
        index.entries.end(),
        [&](const Entry& lhs, const Entry& rhs)
        {
            return compare(view(index, lhs.name), view(index, rhs.name)) < 0;
        }
    );

    return index;
}

void search(const Index& index, std::wstring_view query, std::vector<uint32_t>& out)
{
    out.clear();
    std::vector<uint8_t> found(index.entries.size(), 0);

    for (uint32_t i = 0; i < index.entries.size(); i++)
    {
        const Entry& entry = index.entries[i];
        const std::wstring_view name = view(index, entry.name);

        if (starts_with(name, query))
        {
            out.push_back(i);
            found[i] = 1;
        }
    }

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
