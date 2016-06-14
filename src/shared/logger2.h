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
#include <ctime>
using std::string;

// *** TEST LOGGER ***

class LogEndl
{};

class Logger
{
public:
    Logger(string level, bool header = true) :m_level(level), m_header(header)
    {}

    Logger& operator<< (const LogEndl rhs)
    {
        std::cout << std::endl;
        return *this;
    }

    template <typename T>
    Logger& operator<< (const T& rhs)
    {
        if (m_header)
        {
            time_t timer = time(NULL);
            tm* currtime = localtime(&timer);

            std::cout << "<" << m_level << ">: " << rhs;
            return m_rlogger;
        }
        std::cout << rhs;
        return *this;
    }

private:
    string m_level;
    bool m_header;
    static Logger m_rlogger;
};

extern LogEndl logendl;
void logger2Init();

#endif // !LOGGER2_H_
