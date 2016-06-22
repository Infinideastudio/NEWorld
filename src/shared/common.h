/*
* NEWorld: A free game with similar rules to Minecraft.
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

#ifndef COMMON_H_
#define COMMON_H_

// Compiler flags
#ifdef _MSC_VER
#define NEWORLD_COMPILER_MSVC
#endif

// OS flags
#if defined _WIN32 || defined __CYGWIN__
#define NEWORLD_TARGET_WINDOWS
#define NEWORLD_USE_WINAPI // Windows native API
#endif

#ifdef _DEBUG
#define NEWORLD_DEBUG // Main debug flag
#endif

#define NEWORLD_USE_OPENGL
//#define NEWORLD_USE_DIRECT3D

#ifdef NEWORLD_DEBUG
//    ...
#else
#ifndef NDEBUG
#define NDEBUG // NDEBUG flag for cassert
#endif
#endif

#ifdef NEWORLD_USE_WINAPI
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // Windows API
#else
//    #include <pthread.h> // Or <thread> <mutex>
#endif

#include <cassert>

#if (-1)>>1 == -1
#define NEWORLD_COMPILER_RSHIFT_ARITH // Arithmetic shift right
#endif

constexpr unsigned int NEWorldVersion = 41u;
constexpr const char* CopyrightString = R"(
NEWorld: A free game with similar rules to Minecraft.
Copyright(C) 2016 NEWorld Team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.)";

#endif // !COMMON_H_
