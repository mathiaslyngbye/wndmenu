#ifndef SCAN_HPP
#define SCAN_HPP

#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <future>

#include "tree.hpp"
#include "compare.hpp"

struct StackItem
{
    std::filesystem::path path;
    int depth;
};

static bool valid(std::wstring_view fileName, const std::vector<std::filesystem::path::string_type>& extensions)
{
    const size_t dot = fileName.find_last_of(L'.');
    if (dot == std::wstring_view::npos)
        return false;

    std::wstring_view fileExtension = fileName.substr(dot);
    for (const auto& extension : extensions)
    {
        if (equals(fileExtension, std::wstring_view(extension)))
            return true;
    }
    return false;
}

static ScanResult scanTarget(const Target& target)
{
    ScanResult result;
    result.entries.reserve(8192);
    result.pool.reserve(1 << 20);

    std::vector<StackItem> stack;
    stack.reserve(4096);
    stack.push_back({target.directory, 0});

    std::error_code errorCode;

    while (!stack.empty())
    {
        StackItem current = std::move(stack.back());
        stack.pop_back();

        // Assert search depth
        if ((target.depth >= 0) && (current.depth > target.depth))
            continue;

        std::filesystem::directory_iterator directoryIterator(
            current.path,
            std::filesystem::directory_options::skip_permission_denied,
            errorCode
        );

        if (errorCode)
        {
            errorCode.clear();
            continue; 
        }

        for (const auto& directoryEntry : directoryIterator)
        {
            auto status = directoryEntry.symlink_status(errorCode);
            if (errorCode) 
            {
                errorCode.clear();
                continue;
            }
            else if(status.type() == std::filesystem::file_type::directory)
            {
                stack.push_back({directoryEntry.path(), current.depth + 1});
                continue;
            }
            else if (status.type() != std::filesystem::file_type::regular)
            {
                continue;
            }

            // Assert extension
            const auto& path = directoryEntry.path();
            auto fileName = path.filename().native();
            if (!valid(std::wstring_view(fileName), target.extensions))
                continue;

            // Construct entry
            const size_t dot = fileName.find_last_of(L'.');
            std::wstring_view entryName = (dot == std::wstring::npos)
                ? std::wstring_view(fileName)
                : std::wstring_view(fileName).substr(0, dot);
            auto entryPath = path.native();

            ScanEntry entry;
            entry.name = intern(result.pool, entryName);
            entry.path = intern(result.pool, std::wstring_view(entryPath));
            result.entries.push_back(entry);
        }
    }

    return result;
}

ScanResult scanTargets(const std::vector<Target>& targets)
{
    // Create futures
    std::vector<std::future<ScanResult>> futures;
    futures.reserve(targets.size());
    for (size_t i = 0; i < targets.size(); i++)
    {
        futures.emplace_back(std::async(std::launch::async, [&, i]{
            return scanTarget(targets[i]);
        }));
    }

    // Construct result
    ScanResult result;
    result.entries.reserve(16384);
    result.pool.reserve(1 << 21);
    for (std::future<ScanResult>& future : futures)
        append(result, future.get());

    return result;
}

#endif // SCAN_HPP
