#ifndef STDINCLUDE_H
#define STDINCLUDE_H
#define NEWORLD_USE_WINAPI
#define NERDMODE1
#ifdef NEWORLD_USE_WINAPI
#ifdef NEWORLD_SERVER
#include <thread>
#include <mutex>
using std::thread;
using std::mutex;
#endif
#include <WinSock2.h>
#include <Windows.h>
#else
#include <thread>
#include <mutex>
#endif
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <io.h>
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
#include <direct.h>

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
#include <GL/glext.h>
//FREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H

#endif
#endif