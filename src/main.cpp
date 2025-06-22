#include <iostream>
#include <conio.h> // getch
#include <chrono>


#include "config.hpp"
#include "scan.hpp"

int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    ScanResult result = scanTargets(targets);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Scan time: " << duration << " ms\n";
    
    start = std::chrono::high_resolution_clock::now();
    sort(result.entries);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Sort time: " << duration << " ms\n";

    // Interactive demo
    std::string input;
    char character;

    while (true)
    {
        std::cout << "\r> " << input << "" << std::flush;
        character = getch();

        // Handle backspace or ENTER
        if (character == '\n')
            continue;
        else if (character == '\b' || character == 127)
        {
            if (!input.empty())
                input.pop_back();
        }
        else if (character == 27)
            break;
        else
            input += std::tolower(static_cast<unsigned char>(character));

        // Search and show matches
        auto matches = search(result.entries, input, 10);

        std::cout << "\nMatches for \"" << input << "\":\n";
        for (const ScanEntry* entry : matches)
        {
            std::cout << result.directories[entry->index] << "\\" << entry->name << "\n";
        }
    }

    return 0;
}