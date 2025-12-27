#ifndef COMPARE_HPP
#define COMPARE_HPP

static bool equals(std::wstring_view a, std::wstring_view b)
{
    if (a.size() != b.size())
        return false;

    for (size_t i = 0; i < a.size(); i++)
    {
        wchar_t ca = a[i];
        wchar_t cb = b[i];

        if (ca >= L'A' && ca <= L'Z') 
            ca = ca - L'A' + L'a';

        if (cb >= L'A' && cb <= L'Z')
            cb = cb - L'A' + L'a';

        if (ca != cb)
            return false;
    }

    return true;
}

static int compare(std::wstring_view a, std::wstring_view b)
{
    const size_t n = (a.size() < b.size()) ? a.size() : b.size();
    for (size_t i = 0; i < n; i++)
    {
        wchar_t ca = a[i];
        wchar_t cb = b[i];

        if (ca >= L'A' && ca <= L'Z')
            ca = ca - L'A' + L'a';

        if (cb >= L'A' && cb <= L'Z')
            cb = cb - L'A' + L'a';

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

static bool startsWith(std::wstring_view item, std::wstring_view prefix)
{
    if (prefix.size() > item.size())
        return false;

    for (size_t i = 0; i < prefix.size(); i++)
    {
        wchar_t cs = item[i];
        wchar_t cp = prefix[i];

        if (cs >= L'A' && cs <= L'Z')
            cs = cs - L'A' + L'a';

        if (cp >= L'A' && cp <= L'Z')
            cp = cp - L'A' + L'a';

        if (cs != cp)
            return false;
    }

    return true;
}

#endif