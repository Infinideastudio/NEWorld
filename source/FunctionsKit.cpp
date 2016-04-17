#include "FunctionsKit.h"

using namespace std;
RandGen *mersenne, *linearrand;

extern std::vector<LogItem> Logs;

double rnd()
{
    return mersenne->get_double_co();
}

void RandomGeneratorInit()
{
    mersenne = new MersenneRandGen();
    linearrand = new LinearRandGen();
}

void RandomGeneratorUninit()
{
    delete mersenne;
    delete linearrand;
}

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

string WStringToString(const wstring &s)
{
    string str(s.length(), ' ');
    copy(s.begin(), s.end(), str.begin());
    return str;
}

wstring StringToWString(const string &s)
{
    wstring str(s.length(), ' ');
    copy(s.begin(), s.end(), str.begin());
    return str;
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

#ifdef NEWORLD_TARGET_WINDOWS
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
