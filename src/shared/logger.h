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

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
using std::string;
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/bitwise.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/string.hpp>
#include "common.h"

#define LC "\033["
#define LCOLORT constexpr static const char*
#ifdef NEWORLD_TARGET_WINDOWS
    #define HL "4;4"
    #define LL "24;4"
#else
    #define HL "10"
    #define LL "4"
#endif
namespace LColor
{
    LCOLORT clear = LC"0m";
    LCOLORT alldefault = LC"39;49m"; //reset colors to their defaults

    LCOLORT black = LC"30m";
    LCOLORT red = LC"31m";
    LCOLORT green = LC"32m";
    LCOLORT yellow = LC"33m";
    LCOLORT blue = LC"34m";
    LCOLORT magenta = LC"35m";
    LCOLORT cyan = LC"36m";
    LCOLORT white = LC"37m";
    LCOLORT defaultc = LC"39m";

    LCOLORT highlight = LC"1m";
    LCOLORT lowlight = LC"21m";
    namespace bg
    {
        LCOLORT black = LC LL"0m";
        LCOLORT red = LC LL"1m";
        LCOLORT green = LC LL"2m";
        LCOLORT yellow = LC LL"3m";
        LCOLORT blue = LC LL"4m";
        LCOLORT magenta = LC LL"5m";
        LCOLORT cyan = LC LL"6m";
        LCOLORT white = LC LL"7m";
        namespace highlighted // The highlight code is not in standard and not supported on linux.
        {
            // Using highlighted background color is not recommended since it has many cross-platform problems.
            LCOLORT black = LC HL"0m";
            LCOLORT red = LC HL"1m";
            LCOLORT green = LC HL"2m";
            LCOLORT yellow = LC HL"3m";
            LCOLORT blue = LC HL"4m";
            LCOLORT magenta = LC HL"5m";
            LCOLORT cyan = LC HL"6m";
            LCOLORT white = LC HL"7m";
        }
    }
}
#undef HL
#undef LC
#undef LCOLORT

// Critical levels
enum Level
{
    trace,
    debug,
    info,
    warning,
    error,
    fatal
};

extern int clogLevel; // Minimum critical level using std::clog and output to console
extern int cerrLevel; // Minumum critical level using std::cerr and output to console
extern int fileLevel; // Minumum critical level output to file
extern int lineLevel; // Minumum critical level output the line number of the source file

//specifying if the logger only outputs infomation to files
extern bool fileOnly;

extern std::vector<std::ofstream> fsink;

string getTimeString(char dateSplit, char midSplit, char timeSplit);

// Add a file sink named with current system time
inline void addFileSink(const string& path, const string& prefix);

class Logger
{
public:
    Logger(const char* fileName, int lineNumber, Level level);
    ~Logger();

    template <typename T>
    Logger& operator<< (const T& rhs)
    {
        m_content << rhs;
        return *this;
    }
private:
    std::stringstream m_content;
    Level m_level;
};

void loggerInit(const std::string& prefix);

// information for developers
#define debugstream Logger(__FUNCTION__, __LINE__, debug)
// information for common users
#define infostream Logger(__FUNCTION__, __LINE__, info)
// problems that may affect facility, performance or stability but don't lead the game to crash immediately
#define warningstream Logger(__FUNCTION__, __LINE__, warning)
// the game crashes, but can be resumed by ways such as reloading the world which don't restart the program
#define errorstream Logger(__FUNCTION__, __LINE__, error)
// unrecoverable error and program termination is required
#define fatalstream Logger(__FUNCTION__, __LINE__, fatal)

#endif // !LOGGER_H_
