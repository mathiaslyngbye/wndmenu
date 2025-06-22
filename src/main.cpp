#include <iostream>

#include "config.hpp"
#include "scan.hpp"

int main()
{
    ScanResult result = scan(targets);
    sort(result.entries);

    std::string prefix = "fir";
    std::vector<const ScanEntry*> matches = search(result.entries, prefix, 10);

    for (const ScanEntry* e : matches)
        std::cout << result.directories[e->index] << "\\" << e->name << "\n";

    return 0;
}