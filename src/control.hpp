#ifndef LAUNCHER_HPP
#define LAUNCHER_HPP

#include <string_view>
#include <vector>
#include <cstdint>

struct Index;

Index scan();
void search(const Index& index, std::wstring_view query, std::vector<uint32_t>& out);
void launch(const Index& index, uint32_t id);
std::wstring_view display(const Index& index, uint32_t id);

#endif