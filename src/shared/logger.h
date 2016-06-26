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
    { trace, debug, info, warning, error, fatal, null };
    constexpr static int LevelCount = null;

    static int clogLevel; // Minimum critical level using std::clog and output to console
    static int cerrLevel; // Minumum critical level using std::cerr and output to console
    static int fileLevel; // Minumum critical level output to file
    static int lineLevel; // Minumum critical level output the line number of the source file

    Logger(int level, const char* fileName, int lineNumber) :m_level(level)
    {
        // Level names
        constexpr static const char* LevelString[LevelCount] =
        { "trace", "debug", "info", "warning", "error", "fatal" };
        // Level colors
        constexpr static const CColor::colorfunc LevelColor[LevelCount] =
        { CColor::gray, CColor::gray, CColor::white, CColor::yellow, CColor::red, CColor::dred };
        m_content << CColor::gray << '[' << getTimeString('-', ' ', ':') << ']' << LevelColor[level] << "[" << LevelString[level] << "] ";
        if (level >= lineLevel) m_content << CColor::gray << "(" << fileName << ":" << lineNumber << ") ";
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
    { Logger::fsink.emplace_back(std::ofstream(path + "NEWorld_" + getTimeString('-', '_', '-') + ".log")); }

private:
    int m_level;
    std::stringstream m_content;

    static std::vector<std::ofstream> fsink;
    static string getTimeString(char dateSplit, char midSplit, char timeSplit);
};

void loggerInit();

#define debugstream Logger(Logger::debug, __FUNCTION__, __LINE__)     // 给开发者看的信息
#define infostream Logger(Logger::info, __FUNCTION__, __LINE__)       // 给普通用户看的信息
#define warningstream Logger(Logger::warning, __FUNCTION__, __LINE__) // 可能影响功能、性能、稳定性但是不至于立刻崩溃的问题
#define errorstream Logger(Logger::error, __FUNCTION__, __LINE__)     // 游戏崩溃，但可以通过重新加载世界等方式在不重启程序的情况下解决
#define fatalstream Logger(Logger::fatal, __FUNCTION__, __LINE__)     // 无法恢复的错误，需结束程序

#endif // !LOGGER_H_
