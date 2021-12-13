#pragma once

#include "stdinclude.h"
#include "Typedefs.h"
#include <chrono>
#include <vector>
#include <sstream>
#include <string_view>

extern double stretch;

//常用函数
inline void UITrans(double x, double y) {
    glTranslated(x * stretch, y * stretch, 0);
}

inline void UITrans(int x, int y) {
    glTranslated((static_cast<double>(x)) * stretch, (static_cast<double>(y)) * stretch, 0);
}

inline void UIVertex(double x, double y) {
    glVertex2d(x * stretch, y * stretch);
}

inline void UIVertex(int x, int y) {
    glVertex2i(static_cast<int>(x * stretch), static_cast<int>(y * stretch));
}

extern unsigned int g_seed;

inline unsigned int fastRand() {
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16u) & 0x7FFFu;
}

inline void fastSrand(int seed) { g_seed = seed; }

std::vector<std::string> split(const std::string &str, const std::string &pattern);

inline std::string boolstr(bool b) { return b ? "True" : "False"; }

inline double rnd() { return static_cast<double>(fastRand()) / static_cast<double>(0x7FFFu); }

inline int RoundInt(double d) { return static_cast<int>(lround(d)); }

inline std::string itos(int i) {
    std::stringstream ss;
    ss << i;
    return ss.str();
}

inline bool beginWith(const std::string &str, const std::string &begin) {
    if (str.size() < begin.size()) return false;
    return str.substr(0, begin.size()) == begin;
}

inline bool beginWith(std::string_view str, std::string_view begin) {
    if (str.size() < begin.size()) return false;
    return str.substr(0, begin.size()) == begin;
}

void DebugWarning(const std::string &msg);

void DebugError(const std::string &msg);

template<class T>
void conv(const std::string &str, T &ret) {
    std::stringstream s(str);
    s >> ret;
}

template<class T>
T clamp(T x, T min, T max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline Mutex_t MutexCreate() { return new std::mutex; }

inline void MutexDestroy(Mutex_t _hMutex) { delete _hMutex; }

inline void MutexLock(Mutex_t _hMutex) { _hMutex->lock(); }

inline void MutexUnlock(Mutex_t _hMutex) { _hMutex->unlock(); }

inline Thread_t ThreadCreate(ThreadFunc_t func, void *param) { return new std::thread([=] { func(param); }); }

inline void ThreadWait(Thread_t _hThread) { _hThread->join(); }

inline void ThreadDestroy(Thread_t _hThread) { delete _hThread; }

unsigned int MByteToWChar(wchar_t *dst, const char *src, unsigned int n);

unsigned int WCharToMByte(char *dst, const wchar_t *src, unsigned int n);

inline unsigned int wstrlen(const wchar_t *wstr) { return wcslen(wstr); }

inline void SleepMs(unsigned int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }

inline double timer() {
    return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count()) / 1000.0;
}

//计算距离的平方
inline int DistanceSquare(int ix, int iy, int iz, int x, int y, int z) {
    return (ix - x) * (ix - x) + (iy - y) * (iy - y) + (iz - z) * (iz - z);
}
