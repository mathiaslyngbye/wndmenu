#ifndef INDEX_HPP
#define INDEX_HPP

#include <vector>
#include <string>

#include "compare.hpp"

struct StringRef
{
    uint32_t offset;
    uint32_t length;
};

struct FileEntry
{
    StringRef path; /* Full path + file name */
    StringRef name; /* Searchable name */
};

struct Index
{
    std::vector<FileEntry> entries;
    std::vector<wchar_t> pool;
};

static StringRef intern(std::vector<wchar_t>& pool, std::wstring_view item)
{
    StringRef reference{ (uint32_t)pool.size(), (uint32_t)item.size() };
    pool.insert(pool.end(), item.begin(), item.end());
    pool.push_back(L'\0'); /* Easy to passing to CreateProcess */
    return reference;
}

static std::wstring_view view(const Index& index, StringRef reference)
{
    return std::wstring_view(
        (index.pool.data() + reference.offset),
        reference.length
    );
}

static const wchar_t* c_str(const Index& index, StringRef reference)
{
    return (index.pool.data() + reference.offset);
}

static void append(Index& destination, Index&& source)
{
    const uint32_t size = (uint32_t)destination.pool.size();

    destination.pool.insert(
        destination.pool.end(),
        std::make_move_iterator(source.pool.begin()),
        std::make_move_iterator(source.pool.end())
    );

    destination.entries.reserve(
        (destination.entries.size() + source.entries.size())
    );

    for (FileEntry& entry : source.entries)
    {
        entry.name.offset += size;
        entry.path.offset += size;
        destination.entries.push_back(entry);
    }
}

#endif