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
#include "common.h"
using std::string;

struct ConsoleForeColor
{
    constexpr ConsoleForeColor(bool _r, bool _g, bool _b, bool _i) :r(_r), g(_g), b(_b), i(_i) {}
    bool r, g, b, i;
};

struct ConsoleBackColor
{
    constexpr ConsoleBackColor(bool _r, bool  _g, bool _b, bool  _i) :r(_r), g(_g), b(_b), i(_i) {}
    bool r, g, b, i;
};

namespace CColor
{
    extern unsigned short fg, bg;
    // *** Foreground colors ***
    // Grayscales
    constexpr ConsoleForeColor black(false, false, false, false);
    constexpr ConsoleForeColor dgray(false, false, false, true);
    constexpr ConsoleForeColor gray(true, true, true, false);
    constexpr ConsoleForeColor white(true, true, true, true);
    // Bright colors
    constexpr ConsoleForeColor red(true, false, false, true);
    constexpr ConsoleForeColor green(false, true, false, true);
    constexpr ConsoleForeColor blue(false, false, true, true);
    constexpr ConsoleForeColor yellow(true, true, false, true);
    constexpr ConsoleForeColor cyan(false, true, true, true);
    constexpr ConsoleForeColor magenta(true, false, true, true);
    // Dark colors
    constexpr ConsoleForeColor dred(true, false, false, false);
    constexpr ConsoleForeColor dgreen(false, true, false, false);
    constexpr ConsoleForeColor dblue(false, false, true, false);
    constexpr ConsoleForeColor dyellow(true, true, false, false);
    constexpr ConsoleForeColor dcyan(false, true, true, false);
    constexpr ConsoleForeColor dmagenta(true, false, true, false);

    // *** Background colors ***
    // Grayscales
    constexpr ConsoleBackColor bblack(false, false, false, false);
    constexpr ConsoleBackColor bdgray(false, false, false, true);
    constexpr ConsoleBackColor bgray(true, true, true, false);
    constexpr ConsoleBackColor bwhite(true, true, true, true);
    // Bright colors
    constexpr ConsoleBackColor bred(true, false, false, true);
    constexpr ConsoleBackColor bgreen(false, true, false, true);
    constexpr ConsoleBackColor bblue(false, false, true, true);
    constexpr ConsoleBackColor byellow(true, true, false, true);
    constexpr ConsoleBackColor bcyan(false, true, true, true);
    constexpr ConsoleBackColor bmagenta(true, false, true, true);
    // Dark colors
    constexpr ConsoleBackColor bdred(true, false, false, false);
    constexpr ConsoleBackColor bdgreen(false, true, false, false);
    constexpr ConsoleBackColor bdblue(false, false, true, false);
    constexpr ConsoleBackColor bdyellow(true, true, false, false);
    constexpr ConsoleBackColor bdcyan(false, true, true, false);
    constexpr ConsoleBackColor bdmagenta(true, false, true, false);
}

// Foreground Color
inline std::ostream& operator<< (std::ostream& orig, ConsoleForeColor c)
{
#ifdef NEWORLD_TARGET_WINDOWS
    // Microsoft Windows
#ifdef NEWORLD_USE_WINAPI
    using namespace CColor;
    bool r = c.r, g = c.g, b = c.b, i = c.i;
    fg = (r ? FOREGROUND_RED : 0) |
         (g ? FOREGROUND_GREEN : 0) |
         (b ? FOREGROUND_BLUE : 0)|
         (i ? FOREGROUND_INTENSITY : 0);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), bg | fg);
#endif
    return orig;
#else
    // *nix
    // NOT FINISHED
    return orig << "\033[;m";
#endif
}

// Background Color
inline std::ostream& operator<< (std::ostream& orig, ConsoleBackColor c)
{
#ifdef NEWORLD_TARGET_WINDOWS
    // Microsoft Windows
#ifdef NEWORLD_USE_WINAPI
    using namespace CColor;
    bool r = c.r, g = c.g, b = c.b, i = c.i;
    bg = (r ? BACKGROUND_RED : 0)  |
         (g ? BACKGROUND_GREEN : 0) |
         (b ? BACKGROUND_BLUE : 0) |
         (i ? BACKGROUND_INTENSITY : 0);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), bg | fg);
#endif
    return orig;
#else
    // *nix
    // NOT FINISHED
    return orig << "\033[;m";
#endif
}

class LoggerStream
{
public:
    explicit LoggerStream(bool cerr) :m_cerr(cerr) {}
    ~LoggerStream()
    {
        if (m_cerr)
            std::cerr << m_curr.str() << std::endl;
        else
            std::clog << m_curr.str() << std::endl;
    }

    LoggerStream& operator<< (const ConsoleForeColor& rhs)
    {
        if (m_cerr)
            std::cerr << m_curr.str() << rhs;
        else
            std::clog << m_curr.str() << rhs;
        m_content << m_curr.str();
        m_curr.str("");
        return *this;
    }

    LoggerStream& operator<< (const ConsoleBackColor& rhs)
    {
        if (m_cerr)
            std::cerr << m_curr.str() << rhs;
        else
            std::clog << m_curr.str() << rhs;
        m_content << m_curr.str();
        m_curr.str("");
        return *this;
    }

    template <typename T>
    LoggerStream& operator<< (const T& rhs)
    {
        m_curr << rhs;
        return *this;
    }

    // Get content string
    string get() const
    {
        return m_content.str() + m_curr.str();
    }

private:
    bool m_cerr;
    std::stringstream m_content, m_curr;
};

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

template <int level>
class Logger
{
public:
    Logger(const char* fileName, int lineNumber) :m_content(level >= cerrLevel)
    {
        // Level names
        constexpr std::string levels[] = { "trace","debug","info","warning","error","fatal" };

        // Level colors
        constexpr ConsoleForeColor levelColors[] = { CColor::dgray, CColor::gray, CColor::white, CColor::yellow, CColor::red, CColor::dred };

        // Output to stringstream
        m_content << CColor::dgray << CColor::bblack << '[' << getTimeString('-', ' ', ':') << ']' << levelColors[level] << levels[level];
        if (level >= lineLevel) m_content << CColor::dgray << "(" << fileName << ":" << lineNumber << ")";
        m_content << " " << CColor::gray;
    }

    ~Logger()
    {
        if (level >= fileLevel)
            for (auto &it : fsink)
                it << m_content.get() << std::endl;
    }

    template <typename T>
    Logger& operator<< (const T& rhs)
    {
        m_content << rhs;
        return *this;
    }


private:
    LoggerStream m_content;

};

void loggerInit(const std::string& prefix);

// information for developers
#define debugstream Logger<debug>(__FUNCTION__, __LINE__)
// information for common users
#define infostream Logger<info>(__FUNCTION__, __LINE__)
// problems that may affect facility, performance or stability but don't lead the game to crash immediately
#define warningstream Logger<warning>(__FUNCTION__, __LINE__)
// the game crashes, but can be resumed by ways such as reloading the world which don't restart the program
#define errorstream Logger<error>(__FUNCTION__, __LINE__)
// unrecoverable error and program termination is required
#define fatalstream Logger<fatal>(__FUNCTION__, __LINE__)

extern "C"
{
    NWAPIEXPORT void NWAPICALL consolePrint(char* str, Level level)
    {
        switch(level)
        {
        case trace:
            Logger<trace>(__FUNCTION__, __LINE__) << str;
            break;
        case debug:
            debugstream << str;
            break;
        case warning:
            warningstream << str;
            break;
        case error:
            errorstream << str;
            break;
        case fatal:
            fatalstream << str;
            break;
        default:
        case info:
            infostream << str;
            break;
        }
    }
}

#endif // !LOGGER_H_
