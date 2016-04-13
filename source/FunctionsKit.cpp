#include "FunctionsKit.h"
unsigned int g_seed;
extern std::vector<LogItem> Logs;

vector<string> split(string str, string pattern)
{
    vector<string> ret;
    if (pattern.empty()) return ret;
    size_t start = 0, index = str.find_first_of(pattern, 0);
    while (index != str.npos)
    {
        if (start != index)
            ret.push_back(str.substr(start, index - start));
        start = index + 1;
        index = str.find_first_of(pattern, start);
    }
    if (!str.substr(start).empty())
        ret.push_back(str.substr(start));
    return ret;
}

void DebugWarning(string msg)
{
    Logs.push_back(LogItem(LogItem::Level_Warning, msg));
}

void DebugError(string msg)
{
    Logs.push_back(LogItem(LogItem::Level_Error, msg));
#ifdef _DEBUG
    assert(false);
#endif
}

#ifdef NEWORLD_USE_WINAPI
double timer()
{
    static LARGE_INTEGER counterFreq;
    if (counterFreq.QuadPart == 0)
        QueryPerformanceFrequency(&counterFreq);
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (double)now.QuadPart / counterFreq.QuadPart;
}
#endif
