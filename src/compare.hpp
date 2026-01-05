#ifndef COMPARE_HPP
#define COMPARE_HPP

static constexpr wchar_t fold(wchar_t c) noexcept
{
    return (c >= L'A' && c <= L'Z') ? (c - L'A' + L'a') : c;
}

static bool equals(std::wstring_view a, std::wstring_view b) noexcept
{
    if (a.size() != b.size()) return false;

    for (size_t i = 0; i < a.size(); ++i)
        if (fold(a[i]) != fold(b[i]))
            return false;

    return true;
}

static int compare(std::wstring_view a, std::wstring_view b) noexcept
{
    const size_t n = (a.size() < b.size()) ? a.size() : b.size();

    for (size_t i = 0; i < n; ++i)
    {
        const wchar_t ca = fold(a[i]);
        const wchar_t cb = fold(b[i]);

        if (ca < cb)
            return -1;

        if (ca > cb)
            return 1;
    }

    if (a.size() < b.size())
        return -1;
    if (a.size() > b.size())
        return 1;

    return 0;
}

static bool startsWith(std::wstring_view item, std::wstring_view prefix) noexcept
{
    if (prefix.size() > item.size())
        return false;

    for (size_t i = 0; i < prefix.size(); ++i)
        if (fold(item[i]) != fold(prefix[i]))
            return false;

    return true;
}

static bool endsWith(std::wstring_view item, std::wstring_view suffix) noexcept
{
    if (suffix.size() > item.size())
        return false;

    const size_t offset = item.size() - suffix.size();
    for (size_t i = 0; i < suffix.size(); ++i)
        if (fold(item[offset + i]) != fold(suffix[i]))
            return false;

    return true;
}

#endif