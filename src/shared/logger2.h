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

#ifndef LOGGER2_H_
#define LOGGER2_H_

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
using std::string;

// *** TEST LOGGER ***

namespace Logging
{
    // Level filter not finished

    /*
    enum LogCriticalLevel
    {
        trace, debug, info, warning, error, fatal, LogCriticalLevelCount
    };

    constexpr string LogCriticalLevelString[LogCriticalLevelCount] =
    {
        "trace", "debug", "info", "warning", "error", "fatal"
    };
    */

    extern bool haveFileSink;
    extern std::ofstream fsink;

    string getTimeString(char dateSplit, char midSplit, char timeSplit);
    void initLogger2();

    class Logger
    {
    public:
        Logger(string level)
        { content << getTimeString('-', ' ', ':') << " <" << level << "> "; }
        ~Logger()
        {
            std::cout << content.str() << std::endl;
            if (haveFileSink) fsink << content.str() << std::endl;
        }

        template <typename T>
        Logger& operator<< (const T& rhs)
        {
            content << rhs;
            return *this;
        }
    private:
        std::stringstream content;
    };
}

#define debugstream2 Logging::Logger("debug")     //给开发者看的信息
#define infostream2 Logging::Logger("info")       //给普通用户看的问题
#define warningstream2 Logging::Logger("warning") //可能影响功能、性能、稳定性但是不至于立刻崩溃的问题
#define errorstream2 Logging::Logger("error")     //影响游戏运行的问题
#define fatalstream2 Logging::Logger("fatal")     //无法恢复的错误

#endif // !LOGGER2_H_
