#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// standard include
#define NERDMODE1
// Optional macros above should be set in compiling settings

#ifdef NEWORLD_TARGET_WINDOWS
#include <WinSock2.h>
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

// CROSS PLATFORM REQUIRED `direct.h` and `io.h` are headerers offered by Microsoft
#ifdef NEWORLD_TARGET_WINDOWS
#include <direct.h>
#include <io.h>
#endif

#ifdef NEWORLD_TARGET_MACOSX
#include <unistd.h>
#include <sys/stat.h>
#endif

using std::string;
using std::vector;
using std::pair;
using std::unique_ptr;
using std::map;
using std::cout;
using std::endl;
using std::max;
using std::min;

//GLFW
#define GLFW_DLL
#include <GLFW/glfw3.h>
//GLEXT
#ifdef NEWORLD_TARGET_MACOSX
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
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
typedef HANDLE Mutex_t;
typedef HANDLE Thread_t;
typedef PTHREAD_START_ROUTINE ThreadFunc_t;
#define ThreadFunc DWORD WINAPI
#else
typedef std::mutex* Mutex_t;
typedef std::thread* Thread_t;
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

std::string WStringToString(const std::wstring &s);
std::wstring StringToWString(const std::string &s);

inline int RoundInt(double d)
{
    return int(floor(d + 0.5));
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

#ifdef NEWORLD_TARGET_WINDOWS
inline Mutex_t MutexCreate()
{
    return (Mutex_t)CreateMutex(NULL, FALSE, "");
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
    return (Thread_t)CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, param, 0, NULL);
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
#ifdef NEWORLD_TARGET_MACOSX
    res = mbstowcs(dst, src, n);
#else
    mbstowcs_s(&res, dst, n, src, _TRUNCATE);
#endif
    return res;
}

inline unsigned int WCharToMByte(char* dst, const wchar_t* src, unsigned int n)
{
    size_t res;
#ifdef NEWORLD_TARGET_MACOSX
    res = wcstombs(dst, src, n);
#else
    wcstombs_s(&res, dst, n, src, _TRUNCATE);
#endif
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
#include "logger.h"

enum
{
    version = 39,
    MaxGameTime = 43200,
    NetworkRequestFrequency = 3,
    NetworkRequestMax = 20
};
const string major_version = "Alpha 0.", minor_version = "5", ext_version = " Technical Perview";
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

extern Mutex_t Mutex;
extern Thread_t updateThread;
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

extern RandGen *pRandGen;

#ifdef NEWORLD_DEBUG_PERFORMANCE_REC
extern int c_getChunkPtrFromCPA;
extern int c_getChunkPtrFromSearch;
extern int c_getHeightFromHMap;
extern int c_getHeightFromWorldGen;
#endif

extern Logger GlobalLogger;
#endif
