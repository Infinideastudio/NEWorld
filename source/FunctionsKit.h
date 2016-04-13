#ifndef FUNCTIONSKIT_H
#define FUNCTIONSKIT_H

#include "StdInclude.h"
#include "Typedefs.h"

extern double stretch;

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

inline bool beginWith(const std::wstring& str, const std::wstring& begin)
{
    if (str.size() < begin.size()) return false;
    return str.substr(0, begin.size()) == begin;
}

void DebugWarning(string msg);
void DebugError(string msg);

template<typename T>
inline T extract(const std::wstring& str)
{
    T ret;
    std::wstringstream s(str);
    s >> ret;
    return ret;
}

template<typename T>
inline std::string pack(const T& data)
{
    std::stringstream s;
    s << data;
    return s.str();
}

inline std::string pack(bool data)
{
    return data ? "True" : "False";
}

template<typename T>
inline T clamp(T x, T min_value, T max_value)
{
    return max(min_value, min(x, max_value));
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
    return MultiByteToWideChar(CP_ACP, 0, src, n, dst, n);
}

inline unsigned int WCharToMByte(char* dst, const wchar_t* src, unsigned int n)
{
    return WideCharToMultiByte(CP_ACP, 0, src, n, dst, n * 2, NULL, NULL);
}

inline unsigned int wstrlen(const wchar_t* wstr)
{
    return lstrlenW(wstr);
}

double timer();

inline std::string WChar2Ansi(LPCWSTR pwszSrc)
{
    int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
    if (nLen <= 0) return std::string("");
    char* pszDst = new char[nLen];
    if (NULL == pszDst) return std::string("");
    WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
    pszDst[nLen - 1] = 0;
    std::string strTemp(pszDst);
    delete[] pszDst;
    return strTemp;
}

inline std::string to_string(const std::wstring& inputws)
{
    return WChar2Ansi(inputws.c_str());
}

inline std::wstring Ansi2WChar(LPCSTR pszSrc, int nLen)

{
    int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
    if (nSize <= 0) return NULL;
    WCHAR *pwszDst = new WCHAR[nSize + 1];
    if (NULL == pwszDst) return NULL;
    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, pwszDst, nSize);
    pwszDst[nSize] = 0;
    if (pwszDst[0] == 0xFEFF) // skip Oxfeff
        for (int i = 0; i < nSize; i++)
            pwszDst[i] = pwszDst[i + 1];
    std::wstring wcharString(pwszDst);
    delete pwszDst;
    return wcharString;
}
inline std::wstring to_wstring(const string& s)
{
    return Ansi2WChar(s.c_str(), s.size());
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

inline int Distancen(int ix, int iy, int iz, int x, int y, int z)//Calculte the square of distence between two points
{
    return (ix - x)*(ix - x) + (iy - y)*(iy - y) + (iz - z)*(iz - z);
}
#endif
