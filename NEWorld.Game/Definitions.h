#pragma once
#ifdef NDEBUG
#pragma comment(linker, "/SUBSYSTEM:\"WINDOWS\" /ENTRY:\"mainCRTStartup\"")
#endif

#include "Typedefs.h"
#include "FunctionsKit.h"

#include <string>

//Global Vars
const unsigned int VERSION = 39;
constexpr const char* MAJOR_VERSION = "Alpha 0.";
constexpr const char* MINOR_VERSION = "5";
constexpr const char* EXT_VERSION = " Technical Perview";
constexpr int DefaultWindowWidth = 852; //默认窗口宽度
constexpr int DefaultWindowHeight = 480; //默认窗口高度
extern float FOVyNormal;
extern float mousemove;
extern int viewdistance;
extern int cloudwidth;
extern int selectPrecision;
extern int selectDistance;
extern float walkspeed;
extern float runspeed;
extern int MaxAirJumps;
extern bool SmoothLighting;
extern bool NiceGrass;
extern int linelength;
extern int linedist;
extern float skycolorR;
extern float skycolorG;
extern float skycolorB;
extern float FOVyRunning;
extern float FOVyExt;
extern int Multisample;
extern bool vsync;
extern double stretch;
extern int gametime;
const int gameTimeMax = 43200;

extern int windowwidth;
extern int windowheight;

extern TextureID BlockTextures;
extern TextureID tex_select, tex_unselect, tex_title, tex_mainmenu[6];
extern TextureID DestroyImage[11];
extern TextureID DefaultSkin;

extern Mutex_t Mutex;
extern double lastUpdate;
extern bool updateThreadRun, updateThreadPaused;

extern bool mpclient, mpserver;
extern bool shouldGetScreenshot;
extern bool shouldGetThumbnail;
extern bool FirstUpdateThisFrame;
extern double SpeedupAnimTimer;
extern double TouchdownAnimTimer;
extern double screenshotAnimTimer;
extern double bagAnimTimer;
extern double bagAnimDuration;

extern int GLVersionMajor, GLVersionMinor, GLVersionRev;
extern GLFWwindow *MainWindow;
extern GLFWcursor *MouseCursor;

void AppCleanUp();

