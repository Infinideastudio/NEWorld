// 
// nwcore: Config.h
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#pragma once

#include <boost/predef.h>

// Compiler flags
#ifdef BOOST_COMP_MSVC
#define NEWORLD_COMPILER_MSVC
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#endif

// OS flags
#if (BOOST_OS_CYGWIN || BOOST_OS_WINDOWS)
#define NEWORLD_TARGET_WINDOWS
#elif (BOOST_OS_MACOS || BOOST_OS_IOS)
    #define NEWORLD_TARGET_MACOSX
    #define NEWORLD_TARGET_POSIX
#else
    #define NEWORLD_TARGET_LINUX
    #define NEWORLD_TARGET_POSIX
#endif

#ifdef _DEBUG
#define NEWORLD_DEBUG // Main debug flag
#endif

#if (-1)>>1 == -1
#define NEWORLD_COMPILER_RSHIFT_ARITH // Arithmetic shift right
#endif

// NWAPICALL
#ifdef NEWORLD_COMPILER_MSVC
#define NWAPICALL __cdecl
#elif defined(__i386__) || defined(__i386)
    #define NWAPICALL __attribute__((__cdecl__))
#else
    #undef NWAPICALL
    #define NWAPICALL
#endif

// NWAPIEXPORT
#ifdef NEWORLD_TARGET_WINDOWS
#ifdef NEWORLD_COMPILER_MSVC
#define NWAPIENTRY __declspec(dllimport)
#define NWAPIEXPORT __declspec(dllexport)
#else
#define NWAPIENTRY __attribute__((dllimport))
#define NWAPIEXPORT __attribute__((dllexport))
#endif
#else
#define NWAPIENTRY __attribute__((visibility("default")))
#define NWAPIEXPORT __attribute__((visibility("default")))
#endif

constexpr const char* NEWorldVersionName = "Beta 0.1";
constexpr unsigned short NEWorldVersion = 40u;
constexpr const char* CopyrightString =
    R"(
NEWorld  Copyright (C) 2016  NEWorld Team
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under certain conditions.
For details see "LICENSE".
)";

#if defined(NEWORLD_TARGET_WINDOWS)
constexpr const char* LibSuffix = ".dll";
#else
    constexpr const char* LibSuffix = ".so";
#endif

#ifdef NWCOREEXPORTS
#define NWCOREAPI NWAPIEXPORT
#else
#define NWCOREAPI NWAPIENTRY
#endif
