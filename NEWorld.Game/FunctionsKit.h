#pragma once

#include "stdinclude.h"
#include "Typedefs.h"
#include <chrono>
#include <vector>
#include <sstream>
#include <string_view>

extern double stretch;

//常用函数
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

inline Mutex_t MutexCreate() { return new std::mutex; }

inline void MutexDestroy(Mutex_t _hMutex) { delete _hMutex; }

inline void MutexLock(Mutex_t _hMutex) { _hMutex->lock(); }

inline void MutexUnlock(Mutex_t _hMutex) { _hMutex->unlock(); }

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
