/*
 * NEWorld: An free game with similar rules to Minecraft.
 * Copyright (C) 2016 NEWorld Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// standard include
#define NERDMODE1
// Optional macros above should be set in compiling settings

#ifdef NEWORLD_TARGET_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif //NEWORLD_TARGET_WINDOWS

#include <thread>
#include <mutex>

#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <iomanip>
#include <fstream>
#include <map>
#include <set>
#include <queue>
#include <functional>
#include <algorithm>

#ifdef NEWORLD_TARGET_WINDOWS
#include <direct.h>
#include <io.h>
#elif NEWORLD_TARGET_MACOSX
#include <unistd.h>
#include <sys/stat.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
//#include <OpenGL/gl3.h>
//#include <OpenGL/gl3ext.h>
#endif

using namespace std;

//GLFW
#define GLFW_DLL
#include <GLFW/glfw3.h>

//GLEXT
#ifndef NEWORLD_TARGET_MACOSX
#include <GL/glext.h>
#endif

//FREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H

// typedefs
typedef unsigned char ubyte;
typedef signed char int8;
typedef short int16;
typedef long int32;
typedef long long int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;
typedef unsigned char blockprop;
typedef unsigned char brightness;
typedef unsigned int TextureID;
typedef unsigned int VBOID;
typedef int vtxCount;
typedef int SkinID;
typedef uint64 chunkid;
typedef unsigned int onlineid;

struct block
{
    unsigned short ID;
    union
    {
        struct
        {
            unsigned int OrientionFirst : 5;
            unsigned int OctFirst : 3;
            unsigned int Hex1 : 4;
            unsigned int Hex2 : 4;
        };
        struct
        {
            unsigned int OrientionSecond : 5;
            unsigned int OctSecond : 3;
            unsigned int Data8 : 8;
        };
        struct
        {
            unsigned int Data81 : 8;
            unsigned int Data82 : 8;
        };
        unsigned short Data16;
    };
    block()
        :ID(0), Data16(0) {};
    block(unsigned short iID)
        :ID(iID), Data16(0) {};
    block(unsigned short iID, int iData81, int iData82)
        :ID(iID), Data81(iData81), Data82(iData82) {};

    bool operator == (const block i)
    {
        return i.ID == ID;
    }
    bool operator != (const block i)
    {
        return i.ID != ID;
    }
};

template<typename T = int>
class Vector3D
{
public:
    T x, y, z;
    Vector3D<T>()
        :x(), y(), z()
    { };
    Vector3D<T>(T ix, T iy, T iz)
        :x(ix), y(iy), z(iz)
    { }
    ~Vector3D<T>()
    { }
    Vector3D<T>& operator += (const Vector3D<T>& add)
    {
        x += add.x;
        y += add.y;
        z += add.z;
        return *this;
    }
    Vector3D<T> operator + (const Vector3D<T>& add) const
    {
        Vector3D<T> ret = *this;
        ret += add;
        return ret;
    }
};

namespace Blocks
{
struct BUDDP
{
    block origon;
    block* upd;
    block* slf;
    void* dudp;
    void* dslf;
    int cx, cy, cz;
    BUDDP(block iOri, block* _upd, block* _slf, void* _dudp, void* _dslf,
          int _cx, int _cy, int _cz) :
        origon(iOri), upd(_upd), slf(_slf), dudp(_dudp), dslf(_dslf), cx(_cx), cy(_cy), cz(_cz) {};
    bool operator == (const BUDDP& i)
    {
        return cx == i.cx && cy == i.cy && cz == i.cz;
    }
};

struct TILDP
{
    block* slf;
    void* dslf;
    long long cx, cy, cz;

    TILDP(block* _slf, void* _dslf, long long _cx, long long _cy, long long _cz) :
        slf(_slf), dslf(_dslf), cx(_cx), cy(_cy), cz(_cz) {};
};
}
typedef block item;

#ifdef NEWORLD_TARGET_WINDOWS
typedef HANDLE mutex_t;
typedef HANDLE thread_t;
typedef PTHREAD_START_ROUTINE ThreadFunc_t;
#define ThreadFunc DWORD WINAPI
#else
typedef mutex* mutex_t;
typedef thread* thread_t;
typedef unsigned int(*ThreadFunc_t)(void* param);
#define ThreadFunc unsigned int
#endif

// functions kit
extern double stretch;

vector<string> split(string str, string pattern);

void RandomGeneratorInit();
void RandomGeneratorUninit();

inline int getStretchedIntWindowWidth()
{
    extern int windowwidth;
    return static_cast<int>(windowwidth / stretch);
}
inline int getStretchedIntWindowHeight()
{
    extern int windowheight;
    return static_cast<int>(windowheight / stretch);
}

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

double rnd();
vector<string> split(string str, string pattern);

string WStringToString(const wstring &s);
wstring StringToWString(const string &s);

inline int RoundInt(double d)
{
    return int(floor(d + 0.5));
}

inline bool beginWith(const wstring& str, const wstring& begin)
{
    if (str.size() < begin.size()) return false;
    return str.substr(0, begin.size()) == begin;
}

void logWarning(string msg);
void logError(string msg);

template<typename T>
inline T extract(const wstring& str)
{
    T ret;
    wstringstream s(str);
    s >> ret;
    return ret;
}

template<typename T>
inline string pack(const T& data)
{
    stringstream s;
    s << data;
    return s.str();
}

inline string pack(bool data)
{
    return data ? "True" : "False";
}

template<typename T>
inline T clamp(T x, T min_value, T max_value)
{
    return max(min_value, min(x, max_value));
}

#ifdef NEWORLD_TARGET_WINDOWS
inline mutex_t MutexCreate()
{
    return (mutex_t)CreateMutex(NULL, FALSE, "");
}

inline void MutexDestroy(mutex_t _hMutex)
{
    CloseHandle(_hMutex);
}
inline void MutexLock(mutex_t _hMutex)
{
    WaitForSingleObject(_hMutex, INFINITE);
}

inline void MutexUnlock(mutex_t _hMutex)
{
    ReleaseMutex(_hMutex);
}

inline thread_t ThreadCreate(ThreadFunc_t func, void* param)
{
    return (thread_t)CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, param, 0, NULL);
}

inline void ThreadWait(thread_t _hThread)
{
    WaitForSingleObject(_hThread, INFINITE);
}

inline void ThreadDestroy(thread_t _hThread)
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

#else

inline mutex_t MutexCreate()
{
    return new mutex;
}

inline void MutexDestroy(mutex_t _hMutex)
{
    delete _hMutex;
}

inline void MutexLock(mutex_t _hMutex)
{
    _hMutex->lock();
}

inline void MutexUnlock(mutex_t _hMutex)
{
    _hMutex->unlock();
}

inline thread_t ThreadCreate(ThreadFunc_t func, void* param)
{
    return new thread(func, param);
}

inline void ThreadWait(thread_t _hThread)
{
    _hThread->join();
}

inline void ThreadDestroy(thread_t _hThread)
{
    delete _hThread;
}

inline unsigned int MByteToWChar(wchar_t* dst, const char* src, unsigned int n)
{
    size_t res;
    res = mbstowcs(dst, src, n);
    return res;
}

inline unsigned int WCharToMByte(char* dst, const wchar_t* src, unsigned int n)
{
    size_t res;
    res = wcstombs(dst, src, n);
    return res;
}

inline unsigned int wstrlen(const wchar_t* wstr)
{
    return wcslen(wstr);
}

inline void Sleep(unsigned int ms)
{
#ifdef NEWORLD_TARGET_MACOSX
    unsigned int us = ms * 1000;
    usleep(us);
#else
    unsigned int fr = clock();
    while (clock() - fr <= ms);
#endif
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
#include "logger.h"

enum
{
    version = 39,
    MaxGameTime = 43200,
    NetworkRequestFrequency = 3,
    NetworkRequestMax = 20
};
const string major_version = "Alpha 0.", minor_version = "5", ext_version = " Technical Preview";
extern float FOVyNormal;
extern float mousemove;
extern int ViewDistance;
extern int CloudWidth;
extern int selectPrecision;
extern int selectDistance;
extern float walkspeed;
extern float runspeed;
extern int MaxAirJumps;
extern bool SmoothLighting;
extern bool NiceGrass;
extern bool MergeFace;
extern bool GUIScreenBlur;
extern bool ppistretch;
extern int linelength;
extern int linedist;
extern float skycolorR, skycolorG, skycolorB;
extern float FOVyRunning;
extern float FOVyExt;
extern int Multisample;
extern bool vsync;
extern double stretch;
extern int gametime;

extern int windowwidth;
extern int windowheight;
extern bool gamebegin, gameexit, bagOpened;

extern TextureID BlockTextures, BlockTextures3D;
extern TextureID tex_select, tex_unselect, tex_title, tex_mainmenu[6];
extern TextureID DestroyImage[11];
extern TextureID DefaultSkin;

extern bool multiplayer;
extern string serverip;
extern unsigned short port;

extern mutex_t Mutex;
extern thread_t updateThread;
extern double lastupdate, updateTimer;
extern double lastframe;
extern bool updateThreadRun, updateThreadPaused;

extern bool mpclient, mpserver;
extern bool shouldGetScreenshot;
extern bool shouldGetThumbnail;
extern bool FirstUpdateThisFrame;
extern bool FirstFrameThisUpdate;
extern double SpeedupAnimTimer;
extern double TouchdownAnimTimer;
extern double screenshotAnimTimer;
extern double bagAnimTimer;
extern double bagAnimDuration;

extern int gl_version_major, gl_version_minor, gl_version_rev;
extern GLFWwindow* MainWindow;
extern GLFWcursor* MouseCursor;
extern double mx, my, mxl, myl;
extern int mw, mb, mbp, mbl, mwl;
extern double mxdelta, mydelta;
extern string inputstr;

class RandGen;
extern RandGen* pRandGen;

#endif
