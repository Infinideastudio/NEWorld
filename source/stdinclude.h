#pragma once

//#define NEWORLD_TARGET_WINDOWS
//#define NERDMODE1
// Optional macros above should be set in compiling settings

#ifdef NEWORLD_TARGET_WINDOWS

#ifdef NEWORLD_SERVER
#include <thread>
#include <mutex>
using std::thread;
using std::mutex;
#endif //NEWORLD_SERVER
#include <WinSock2.h>
#include <Windows.h>

#else //NEWORLD_TARGET_WINDOWS

#include <thread>
#include <mutex>

#endif //NEWORLD_TARGET_WINDOWS

#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
// CROSS PLATFORM REQUIRED io.h is a header offered by Microsoft
#ifdef NEWORLD_TARGET_WINDOWS
#include <io.h>
#endif
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
#include <cassert>
#include <stdarg.h>
// CROSS PLATFORM REQUIRED direct.h is a header offered by Microsoft
#ifdef NEWORLD_TARGET_WINDOWS
#include <direct.h>
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

#ifdef NEWORLD_GAME

//GLFW
#define GLFW_DLL
#include <GLFW/glfw3.h>
//GLEXT
#ifdef NEWORLD_TARGET_MACOSX
#include <OpenGL.h>
#include <gl3ext.h>
#else
#include <GL/glext.h>
#endif
//FREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H

#endif //NEWORLD_GAME
