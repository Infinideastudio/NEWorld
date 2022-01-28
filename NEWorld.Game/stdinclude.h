#pragma once

#include <thread>
#include <mutex>

#define _USE_MATH_DEFINES
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

#include <cmath>
#include <ctime>
#include <vector>
#include <cassert>
#include <cstdarg>

#ifdef NEWORLD_GAME

#define GLEW_NO_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#endif