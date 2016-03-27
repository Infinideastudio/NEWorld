#include "Definitions.h"

//Global Vars
float FOVyNormal = 60.0f;       //视野角度
float mousemove = 0.2f;         //鼠标灵敏度
int ViewDistance = 8;           //视野距离
int CloudWidth = 10;            //云的宽度
int selectPrecision = 32;       //选择方块的精度
int selectDistance = 8;         //选择方块的距离
float walkspeed = 0.15f;        //玩家前进速度
float runspeed = 0.3f;          //玩家跑步速度
int MaxAirJumps = 3 - 1;        //空中N段连跳
bool SmoothLighting = true;     //平滑光照
bool NiceGrass = true;          //草地材质连接
bool MergeFace = false;         //合并面渲染
bool GUIScreenBlur = false;     //GUI背景模糊  Void:这个算法慢死了，我关掉了  qiaozhanrong:23333我也想关掉
int linelength = 10;            //跟F3的准星有关。。。
int linedist = 30;              //跟F3的准星有关。。。
bool ppistretch = false;        //试验功能，默认关闭
float skycolorR = 0.7f;         //天空颜色Red
float skycolorG = 1.0f;         //天空颜色Green
float skycolorB = 1.0f;         //天空颜色Blue
float FOVyRunning = 8.0f;
float FOVyExt;
double stretch = 1.0f;          //ppi缩放比例（供gui绘制使用）
int Multisample = 0;            //多重采样抗锯齿
bool vsync = false;             //垂直同步
int gametime = 0;                //游戏时间 0~2592000
//float daylight;

int windowwidth;     //窗口宽度
int windowheight;    //窗口宽度
bool gamebegin, gameexit, bagOpened;

//多人游戏
bool multiplayer = false;
string serverip;
unsigned short port = 30001;

TextureID BlockTextures, BlockTextures3D;
TextureID tex_select, tex_unselect, tex_title, tex_mainmenu[6];
TextureID DestroyImage[11];
TextureID DefaultSkin;

//线程
Mutex_t Mutex;
Thread_t updateThread;
double lastupdate, updateTimer;
double lastframe;
bool updateThreadRun, updateThreadPaused;

bool shouldGetScreenshot;
bool shouldGetThumbnail;
bool FirstUpdateThisFrame;
bool FirstFrameThisUpdate;
double SpeedupAnimTimer;
double TouchdownAnimTimer;
double screenshotAnimTimer;
double bagAnimTimer;
double bagAnimDuration = 0.5;

//OpenGL
int GLVersionMajor, GLVersionMinor, GLVersionRev;
//GLFW
GLFWwindow* MainWindow;
GLFWcursor* MouseCursor;

//鼠标输入数据
double mx, my, mxl, myl;
int mw, mb, mbp, mbl, mwl;
double mxdelta, mydelta;
//键盘输入数据
string inputstr;

#ifdef NEWORLD_DEBUG_PERFORMANCE_REC
int c_getChunkPtrFromCPA;
int c_getChunkPtrFromSearch;
int c_getHeightFromHMap;
int c_getHeightFromWorldGen;
#endif

