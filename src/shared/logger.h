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
#include "common.h"

class ConsoleColor
{
public:
    bool r, g, b, i;

    constexpr ConsoleColor() :r(false), g(false), b(false), i(false)
    {}
    constexpr ConsoleColor(bool r_, bool g_, bool b_, bool i_) :r(r_), g(g_), b(b_), i(i_)
    {}

    string get() const
    {
#ifdef NEWORLD_TARGET_WINDOWS
        // Microsoft Windows
        WORD col = 0u;
        if (r) col |= FOREGROUND_RED;
        if (g) col |= FOREGROUND_GREEN;
        if (b) col |= FOREGROUND_BLUE;
        if (i) col |= FOREGROUND_INTENSITY;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), col);
        return "";
#else
        // *nix
        // NOT FINISHED
        return "\033[;m";
#endif
    }
};

namespace CColor
{
    // Grayscales
    constexpr ConsoleColor black(false, false, false, false);
    constexpr ConsoleColor dgray(false, false, false, true);
    constexpr ConsoleColor gray(true, true, true, false);
    constexpr ConsoleColor white(true, true, true, true);
    // Bright colors
    constexpr ConsoleColor red(true, false, false, true);
    constexpr ConsoleColor green(false, true, false, true);
    constexpr ConsoleColor blue(false, false, true, true);
    constexpr ConsoleColor yellow(true, true, false, true);
    constexpr ConsoleColor cyan(false, true, true, true);
    constexpr ConsoleColor magenta(true, false, true, true);
    // Dark colors
    constexpr ConsoleColor dred(true, false, false, false);
    constexpr ConsoleColor dgreen(false, true, false, false);
    constexpr ConsoleColor dblue(false, false, true, false);
    constexpr ConsoleColor dyellow(true, true, false, false);
    constexpr ConsoleColor dcyan(false, true, true, false);
    constexpr ConsoleColor dmagenta(true, false, true, false);
}

class LoggerStream
{
public:
    explicit LoggerStream(bool cerr) :m_cerr(cerr)
    {}

    LoggerStream& operator<< (const ConsoleColor& rhs)
    {
        if (m_cerr) std::cerr << rhs.get();
        else std::clog << rhs.get();
        return *this;
    }

    template <typename T>
    LoggerStream& operator<< (const T& rhs)
    {
        if (m_cerr) std::cerr << rhs;
        else std::clog << rhs;
        m_content << rhs;
        return *this;
    }

    // Get content string
    const string get()
    { return m_content.str(); }

private:
    bool m_cerr;
    std::stringstream m_content;
};

class Logger
{
public:
    // Critical levels
    enum Level
    {
        trace,
        debug,
        info,
        warning,
        error,
        fatal,
        null
    };
    constexpr static int LevelCount = null;

    // Level names
    constexpr static const char* LevelString[LevelCount] =
    {
        "trace",
        "debug",
        "info",
        "warning",
        "error",
        "fatal"
    };

    // Level colors
    constexpr static const ConsoleColor LevelColor[LevelCount] =
    {
        CColor::dgray,
        CColor::gray,
        CColor::white,
        CColor::yellow,
        CColor::red,
        CColor::dred
    };

    static int clogLevel; // Minimum critical level using std::clog and output to console
    static int cerrLevel; // Minumum critical level using std::cerr and output to console
    static int fileLevel; // Minumum critical level output to file
    static int lineLevel; // Minumum critical level output the line number of the source file

    Logger(int level, const char* fileName, int lineNumber) :m_level(level), m_content(m_level >= cerrLevel)
    {
        m_content << CColor::dgray << '[' << getTimeString('-', ' ', ':') << ']' << LevelColor[level] << "[" << LevelString[level] << "] ";
        if (level >= lineLevel) m_content << CColor::dgray << "(" << fileName << ":" << lineNumber << ") ";
        m_content << CColor::gray;
    }

    ~Logger()
    {
        if (m_level >= cerrLevel) std::cerr << std::endl;
        else if (m_level >= clogLevel) std::clog << std::endl;
        if (m_level >= fileLevel)
            for (auto &it : fsink) it << m_content.get() << std::endl;
    }

    template <typename T>
    Logger& operator<< (const T& rhs)
    {
        m_content << rhs;
        return *this;
    }

    // Add a file sink named with current system time
    static void addFileSink(const string& path)
    {
        Logger::fsink.emplace_back(path + "NEWorld_" + getTimeString('-', '_', '-') + ".log");
    }

private:
    int m_level;
    LoggerStream m_content;

    static std::vector<std::ofstream> fsink;
    static string getTimeString(char dateSplit, char midSplit, char timeSplit);
};

void loggerInit();

// information for developers
#define debugstream Logger(Logger::debug, __FUNCTION__, __LINE__)
// information for common users
#define infostream Logger(Logger::info, __FUNCTION__, __LINE__)
// problems that may affect facility, performance or stability but don't lead the game to crash immediately
#define warningstream Logger(Logger::warning, __FUNCTION__, __LINE__)
// the game crashes, but can be resumed by ways such as reloading the world which don't restart the program
#define errorstream Logger(Logger::error, __FUNCTION__, __LINE__)
// unrecoverable error and program termination is required
#define fatalstream Logger(Logger::fatal, __FUNCTION__, __LINE__)

#endif // !LOGGER_H_
