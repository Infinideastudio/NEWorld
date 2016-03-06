#pragma once
#include "StdInclude.h"
#include "Typedefs.h"

extern double stretch;

//常用函数
vector<string> split(string str, string pattern);

inline void UITrans(double x, double y)
{
    glTranslated(x*stretch, y*stretch, 0);
}
inline void UITrans(int x, int y)
{
    glTranslated((static_cast<double>(x))*stretch, (static_cast<double>(y))*stretch, 0);
}
inline void UIVertex(double x, double y)
{
    glVertex2d(x*stretch, y*stretch);
}
inline void UIVertex(int x, int y)
{
    glVertex2i(static_cast<int>(x*stretch), static_cast<int>(y*stretch));
}

extern unsigned int g_seed;
inline int fastRand()
{
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}
inline void fastSrand(int seed)
{
    g_seed = seed;
}
vector<string> split(string str, string pattern);
inline string boolstr(bool b)
{
    return b ? "True" : "False";
}
inline double rnd()
{
    return (double)fastRand() / (RAND_MAX + 1);
}
inline int RoundInt(double d)
{
    return int(floor(d + 0.5));
}

inline string itos(int i)
{
    std::stringstream ss;
    ss << i;
    return string(ss.str());
}

inline bool beginWith(string str, string begin)
{
    if (str.size() < begin.size()) return false;
    return str.substr(0, begin.size()) == begin;
}

inline void DebugWarning(string msg)
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

inline void DebugError(string msg)
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

template<class T> inline void conv(string str, T& ret)
{
    std::stringstream s(str);
    s >> ret;
}

template<class T> inline T clamp(T x, T min, T max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

#ifdef NEWORLD_USE_WINAPI
inline Mutex_t MutexCreate()
{
    return CreateMutex(NULL, FALSE, "");
}
inline void MutexDestroy(Mutex_t _hMutex)
{
    CloseHandle(_hMutex);
}
inline void MutexLock(Mutex_t _hMutex)
{
    WaitForSingleObject(_hMutex, INFINITE);
}
inline void MutexUnlock(Mutex_t _hMutex)
{
    ReleaseMutex(_hMutex);
}
inline Thread_t ThreadCreate(ThreadFunc_t func, void* param)
{
    return CreateThread(NULL, 0, func, param, 0, NULL);
}
inline void ThreadWait(Thread_t _hThread)
{
    WaitForSingleObject(_hThread, INFINITE);
}
inline void ThreadDestroy(Thread_t _hThread)
{
    CloseHandle(_hThread);
}
inline unsigned int MByteToWChar(wchar_t* dst, const char* src, unsigned int n)
{
    int res = MultiByteToWideChar(CP_ACP, 0, src, n, dst, n);
    return res;
}
inline unsigned int WCharToMByte(char* dst, const wchar_t* src, unsigned int n)
{
    return WideCharToMultiByte(CP_ACP, 0, src, n, dst, n * 2, NULL, NULL);
}
inline unsigned int wstrlen(const wchar_t* wstr)
{
    return lstrlenW(wstr);
}
inline double timer()
{
    static LARGE_INTEGER counterFreq;
    if (counterFreq.QuadPart == 0) QueryPerformanceFrequency(&counterFreq);
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (double)now.QuadPart / counterFreq.QuadPart;
}
#else
inline Mutex_t MutexCreate()
{
    return new std::mutex;
}
inline void MutexDestroy(Mutex_t _hMutex)
{
    delete _hMutex;
}
inline void MutexLock(Mutex_t _hMutex)
{
    _hMutex->lock();
}
inline void MutexUnlock(Mutex_t _hMutex)
{
    _hMutex->unlock();
}
inline Thread_t ThreadCreate(ThreadFunc_t func, void* param)
{
    return new std::thread(func, param);
}
inline void ThreadWait(Thread_t _hThread)
{
    _hThread->join();
}
inline void ThreadDestroy(Thread_t _hThread)
{
    delete _hThread;
}
inline unsigned int MByteToWChar(wchar_t* dst, const char* src, unsigned int n)
{
    size_t res;
    mbstowcs_s(&res, dst, n, src, _TRUNCATE);
    return res;
}
inline unsigned int WCharToMByte(char* dst, const wchar_t* src, unsigned int n)
{
    size_t res;
    wcstombs_s(&res, dst, n, src, _TRUNCATE);
    return res;
}
inline unsigned int wstrlen(const wchar_t* wstr)
{
    return wcslen(wstr);
}
inline void Sleep(unsigned int ms)
{
    unsigned int fr = clock();
    while (clock() - fr <= ms);
}
inline double timer()
{
    return (double)clock() / CLOCKS_PER_SEC;
}
#endif
inline int Distancen(int ix, int iy, int iz, int x, int y, int z)//计算距离的平方
{
    return (ix - x)*(ix - x) + (iy - y)*(iy - y) + (iz - z)*(iz - z);
}