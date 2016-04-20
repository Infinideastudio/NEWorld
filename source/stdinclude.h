#ifndef STDINCLUDE_H
#define STDINCLUDE_H

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

#endif
