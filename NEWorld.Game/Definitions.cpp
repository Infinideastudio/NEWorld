#include "Definitions.h"

//Global Vars
float FOVyNormal = 60.0f;
float mousemove = 0.2f;
int viewdistance = 8;
int cloudwidth = 10;
int selectPrecision = 32;
int selectDistance = 8;
float walkspeed = 0.15f;
float runspeed = 0.3f;
int MaxAirJumps = 3 - 1;
bool SmoothLighting = true;
bool NiceGrass = true;
int linelength = 10;
int linedist = 30;
float skycolorR = 0.7f;
float skycolorG = 1.0f;
float skycolorB = 1.0f;
float FOVyRunning = 8.0f;
float FOVyExt;
double stretch = 1.0f;
int Multisample = 0;
bool vsync = false;
int gametime = 0;
//float daylight;

int windowwidth;
int windowheight;

//������Ϸ
bool multiplayer = false;
std::string serverip;
unsigned short port = 30001;

TextureID BlockTextures;
TextureID tex_select, tex_unselect, tex_mainmenu[6];
TextureID DestroyImage[11];
TextureID DefaultSkin;

Mutex_t Mutex;
double lastUpdate;
bool updateThreadRun, updateThreadPaused;

bool shouldGetScreenshot;
bool shouldGetThumbnail;
bool FirstUpdateThisFrame;
double SpeedupAnimTimer;
double TouchdownAnimTimer;
double screenshotAnimTimer;
double bagAnimTimer;
double bagAnimDuration = 0.5;

//OpenGL
int GLVersionMajor, GLVersionMinor, GLVersionRev;
//GLFW
GLFWwindow *MainWindow;
GLFWcursor *MouseCursor;

double mx, my, mxl, myl;
int mw, mb, mbp, mbl, mwl;
double mxdelta, mydelta;
std::string inputstr;

#ifdef NEWORLD_DEBUG_PERFORMANCE_REC
int c_getChunkPtrFromCPA;
int c_getChunkPtrFromSearch;
int c_getHeightFromHMap;
int c_getHeightFromWorldGen;
#endif

