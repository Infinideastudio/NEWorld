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

class Logger
{
public:
    // Critical levels
    enum Level
    { trace, debug, info, warning, error, fatal, null };
    constexpr static int LevelCount = null;
    // Level names
    constexpr static const char* LevelString[LevelCount] =
    { "trace", "debug", "info", "warning", "error", "fatal" };

    static int clogLevel; // Minimum critical level using std::clog and output to console
    static int cerrLevel; // Minumum critical level using std::cerr and output to console
    static int fileLevel; // Minumum critical level output to file

    Logger(int level) :m_level(level)
    { m_content << getTimeString('-', ' ', ':') << " <" << LevelString[level] << "> "; }

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

#define debugstream Logger(Logger::debug)     //给开发者看的信息
#define infostream Logger(Logger::info)       //给普通用户看的问题
#define warningstream Logger(Logger::warning) //可能影响功能、性能、稳定性但是不至于立刻崩溃的问题
#define errorstream Logger(Logger::error)     //影响游戏运行的问题
#define fatalstream Logger(Logger::fatal)     //无法恢复的错误

#endif // !LOGGER_H_
