#include "FunctionsKit.h"
unsigned int g_seed;

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
#ifdef NEWORLD_USE_WINAPI
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    printf("[Debug][Warning]");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    printf("%s\n", msg.c_str());
#else
    printf("[Debug][Warning]%s\n", msg.c_str());
#endif
}

void DebugError(string msg)
{
#ifdef NEWORLD_USE_WINAPI
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    printf("[Debug][Error]");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    printf("%s\n", msg.c_str());
#else
    printf("[Debug][Error]%s\n", msg.c_str());
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
