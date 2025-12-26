#ifndef SCAN_HPP
#define SCAN_HPP

#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <future>

#include "tree.hpp"

ScanResult scanTarget(const Target& target)
{
    // Initialize and reserve
    std::error_code error;
    ScanResult result;
    result.entries.reserve(2048);
    result.directories.reserve(256);
    std::unordered_map<std::string, uint32_t> map;
    map.reserve(256);

    // Process directory stack
    std::vector<std::filesystem::path> stack = { target.directory };
    while (!stack.empty())
    {
        // Dequeue path
        std::filesystem::path path = std::move(stack.back());
        stack.pop_back();

        // Assert depth
        std::filesystem::path relative = std::filesystem::relative(path, target.directory, error);
        int depth = (error ? 0 : static_cast<int>(std::distance(relative.begin(), relative.end())));
        if (target.depth >= 0 && depth > target.depth)
            continue;

        // Create and assert iterator
        std::filesystem::directory_iterator entries(path, std::filesystem::directory_options::skip_permission_denied, error);
        if (error) 
            continue;

        // Check directory entries
        for (const std::filesystem::directory_entry& entry : entries)
        {
            // Add subdirectories to stack
            if (entry.is_directory())
            {
                stack.push_back(entry.path());
                continue;
            } 

            // Add files to results
            if (entry.is_regular_file()) 
            {
                // Find extension
                std::string extension = entry.path().extension().string();
                if (std::find(target.extensions.begin(), target.extensions.end(), extension) == target.extensions.end())
                    continue;

                // Add source if new exist
                std::string source  = entry.path().parent_path().string();
                auto [iterator, inserted] = map.try_emplace(source, static_cast<uint32_t>(result.directories.size()));
                if (inserted)
                    result.directories.emplace_back(std::move(source));
                
                // Add entry
                std::string name = entry.path().filename().string();
                std::transform(name.begin(), name.end(), name.begin(), ::tolower);
                uint32_t index = iterator->second;
                result.entries.emplace_back(ScanEntry{name, index});
            }
        }
    }
    
    // Return
    return result;
}

ScanResult scanTargets(const std::vector<Target>& targets)
{
    // Add scan calls to futures
    std::vector<std::future<ScanResult>> futures;
    futures.reserve(targets.size());
    for (const Target& target : targets)
        futures.push_back(std::async(std::launch::async, scanTarget, std::ref(target)));

    // Initialize and reserve
    ScanResult result;
    result.entries.reserve(4096);
    result.directories.reserve(256);

    std::unordered_map<std::string, uint32_t> map;
    map.reserve(256);

    for (std::future<ScanResult> &future : futures)
    {
        // Get partial result
        ScanResult part = future.get();

        // Merge directories
        for (const std::string &directory : part.directories)
        {
            auto [iterator, inserted] = map.try_emplace(directory, static_cast<uint32_t>(result.directories.size()));
            if (inserted)
                result.directories.emplace_back(directory);
        }

        // Reserve more memory as needed
        const size_t needed = result.entries.size() + part.entries.size();
        if (needed > result.entries.capacity())
            result.entries.reserve(needed);
        
        // Merge entries with remapped directory indices
        for (const ScanEntry &entry : part.entries)
        {
            uint32_t remapped = map[part.directories[entry.index]]; // or use find()
            result.entries.emplace_back(ScanEntry{entry.name, remapped});
        }
    }

    // Return
    return result;
}

void sort(std::vector<ScanEntry> &entries)
{
    // Final sort for consistent search
    std::sort(entries.begin(), entries.end(),
        [](const ScanEntry& a, const ScanEntry& b)
        {
            return a.name < b.name;
        }
    );
}

std::vector<const ScanEntry*> search(const std::vector<ScanEntry>& entries, const std::string& prefix, std::size_t cap) 
{
    std::vector<const ScanEntry*> results;
    results.reserve(cap);

    if (prefix.empty())
    {
        // Return first entries directly
        for (const ScanEntry& entry : entries)
        {
            results.push_back(&entry);
            if (results.size() >= cap)
                break;
        }
        return results;
    }

    // Binary search to find start of matches
    auto begin = std::lower_bound(
        entries.begin(), entries.end(), prefix,
        [](const ScanEntry& entry, const std::string& value)
        {
            return entry.name.compare(0, value.size(), value) < 0;
        }
    );

    // Collect up to cap matches
    for (auto iterator = begin; iterator != entries.end(); ++iterator)
    {
        if (iterator->name.size() < prefix.size())
            break;

        if (iterator->name.compare(0, prefix.size(), prefix) == 0)
        {
            results.push_back(&*iterator);
            if (results.size() >= cap)
                break;
        } 
        else
            break;
    }

    return results;
}


#endif // SCAN_HPP
