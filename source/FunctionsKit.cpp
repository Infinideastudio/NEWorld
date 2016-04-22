#include "Definitions.h"

using namespace std;
RandGen *pRandGen;

double rnd()
{
    return pRandGen->get_double_co();
}

#if (defined NEWORLD_TARGET_MACOSX) && (defined __RDRND__)
void RandomGeneratorInit()
{
    pRandGen = new IntelRandGen();
}
#else
void RandomGeneratorInit()
{
    pRandGen = new MersenneRandGen();
}
#endif

void RandomGeneratorUninit()
{
    delete pRandGen;
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
