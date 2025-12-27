#ifndef TREE_HPP
#define TREE_HPP

#include <vector>

#include "compare.hpp"

struct PoolReference
{
    uint32_t offset;
    uint32_t length;
};

struct ScanEntry
{
    PoolReference path;
    PoolReference name;
};

struct ScanResult
{
    std::vector<ScanEntry> entries;
    std::vector<wchar_t> pool;
};

static PoolReference intern(std::vector<wchar_t>& pool, std::wstring_view item)
{
    PoolReference reference{ (uint32_t)pool.size(), (uint32_t)item.size() };
    pool.insert(pool.end(), item.begin(), item.end());
    pool.push_back(L'\0'); // Make it easy to pass to CreateProcess
    return reference;
}

static std::wstring_view view(const ScanResult& result, PoolReference reference)
{
    return std::wstring_view(
        (result.pool.data() + reference.offset),
        reference.length
    );
}

static const wchar_t* c_str(const ScanResult& result, PoolReference reference)
{
    return result.pool.data() + reference.offset;
}

static void append(ScanResult& destination, ScanResult&& source)
{
    const uint32_t base = (uint32_t)destination.pool.size();

    destination.pool.insert(
        destination.pool.end(),
        std::make_move_iterator(source.pool.begin()),
        std::make_move_iterator(source.pool.end())
    );

    destination.entries.reserve(
        destination.entries.size() + source.entries.size()
    );

    for (ScanEntry& entry : source.entries)
    {
        entry.name.offset += base;
        entry.path.offset += base;
        destination.entries.push_back(entry);
    }
}

static std::vector<const ScanEntry*> prefixSearch(
    const ScanResult& result,
    std::wstring_view prefix,
    size_t limit)
{
    std::vector<const ScanEntry*> out;
    out.reserve(limit);

    for (const ScanEntry& entry : result.entries)
    {
        std::wstring_view name = view(result, entry.name);
        if (startsWith(name, prefix))
        {
            out.push_back(&entry);
            if (out.size() >= limit)
                break;
        }
    }

    return out;
}


#endif