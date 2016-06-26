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
#include "consolecolor.h"

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
        fatal
    };

    static int clogLevel; // Minimum critical level using std::clog and output to console
    static int cerrLevel; // Minumum critical level using std::cerr and output to console
    static int fileLevel; // Minumum critical level output to file
    static int lineLevel; // Minumum critical level output the line number of the source file

    Logger(int level, const char* fileName, int lineNumber) :m_level(level)
    {
        using namespace CColor;
        // Level names
        constexpr char* LevelString[] =
        {
            "trace",
            "debug",
            "info",
            "warning",
            "error",
            "fatal"
        };
        // Level colors
        constexpr CColor::colorfunc LevelColor[] =
        {
            color<Color::gray>,
            color<Color::gray>,
            color<Color::white>,
            color<Color::yellow>,
            color<Color::red>,
            color<Color::dred>
        };
        m_content << color<Color::gray> << '[' << getTimeString('-', ' ', ':') << ']' << LevelColor[level] << "[" << LevelString[level] << "] ";
        if (level >= lineLevel)
            m_content << color<Color::gray> << "(" << fileName << ":" << lineNumber << ") ";
    }

    ~Logger()
    {
        if (m_level >= cerrLevel) std::cerr << m_content.str() << std::endl;
        else if (m_level >= clogLevel) std::clog << m_content.str() << std::endl;
        if (m_level >= fileLevel)
            for (auto &it : fsink) it << m_content.str() << std::endl;
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
    std::stringstream m_content;

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
