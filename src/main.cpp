#include <iostream>
#include <conio.h> // getch

#include "config.hpp"
#include "scan.hpp"

int main()
{
    ScanResult result = scan(targets);
    sort(result.entries);

    // Interactive demo
    std::string input;
    char character;

    while (true) {
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