#include "FunctionsKit.h"

#if __has_include(<Windows.h>)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NEWORLD_WIN32
#include <Windows.h>
#endif

unsigned int g_seed = 0;

std::vector<std::string> split(const std::string &str, const std::string &pattern) {
    std::vector<std::string> ret;
    if (pattern.empty()) return ret;
    size_t start = 0, index = str.find_first_of(pattern, 0);
    while (index != str.npos) {
        if (start != index)
            ret.push_back(str.substr(start, index - start));
        start = index + 1;
        index = str.find_first_of(pattern, start);
    }
    if (!str.substr(start).empty())
        ret.push_back(str.substr(start));
    return ret;
}

unsigned int MByteToWChar(wchar_t *dst, const char *src, unsigned int n) {
#ifdef NEWORLD_WIN32
    return MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, src, n, dst, n);
#else
    return mbstowcs(dst, src, n);
#endif
}

unsigned int WCharToMByte(char *dst, const wchar_t *src, unsigned int n) {
#ifdef NEWORLD_WIN32
    return WideCharToMultiByte(CP_UTF8, WC_COMPOSITECHECK, src, n, dst, n, nullptr, nullptr);
#else
    return wcstombs(dst, src, n);
#endif
}
