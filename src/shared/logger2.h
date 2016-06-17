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
#include <iomanip>
#include <ctime>
using std::string;

// *** TEST LOGGER ***

class Logger
{
public:
    Logger(string level)
    {
        time_t timer = time(NULL);
        tm* currtime = localtime(&timer); // DO NOT `delete` THIS POINTER!
        std::cout << std::setfill('0') << std::setw(4) << currtime->tm_year + 1900 << "-" << std::setw(2) << currtime->tm_mon + 1 << "-" << std::setw(2) << currtime->tm_mday
                  << " " << std::setw(2) << currtime->tm_hour << ":" << std::setw(2) << currtime->tm_min << ":" << std::setw(2) << currtime->tm_sec
                  << ": <" << level << "> ";
    }
    ~Logger()
    { std::cout << std::endl; }

    template <typename T>
    Logger& operator<< (const T& rhs)
    {
        std::cout << rhs;
        return *this;
    }
};

#define debugstream2 Logger("debug")     //给开发者看的信息
#define infostream2 Logger("info")       //给普通用户看的问题
#define warningstream2 Logger("warning") //可能影响功能、性能、稳定性但是不至于立刻崩溃的问题
#define errorstream2 Logger("error")     //影响游戏运行的问题
#define fatalstream2 Logger("fatal")     //无法恢复的错误

#endif // !LOGGER2_H_
