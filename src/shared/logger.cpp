/*
 * NEWorld: An free game with similar rules to Minecraft.
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

#include "logger.h"
#include <assert.h>
#include <ctime>
#include <sstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

void Logger::log(std::string information, CriticalLevel level)
{
    std::stringstream ss;

    using namespace std::chrono;
    auto now = system_clock::now();
    time_t t = system_clock::to_time_t(now);
    auto time = std::put_time(std::localtime(&t), "%Y/%m/%d %H:%M:%S");
    ss << '[' << time << "] ";
    switch (level)
    {
    case CRITICAL_LEVEL_INFORMATION:
        ss << "(II) ";
        break;
    case CRITICAL_LEVEL_WARNING:
        ss << "(WW) ";
        break;
    case CRITICAL_LEVEL_ERROR:
        ss << "(EE) ";
        break;
    case CRITICAL_LEVEL_NOT_IMPLEMENTED:
        ss << "(NI) ";
        break;
    default:
        ss << "(?\?) ";
    }
    ss << information << std::endl;
    m_logs.push_back(ss.str());

    if (m_displayToConsole)
    {
        if (level == CRITICAL_LEVEL_ERROR)
        {
            std::cerr << ss.str();
        }
        else
        {
            std::cout << ss.str();
        }
    }
}

std::string Logger::exportAll()
{
    std::string ret;
    for (std::vector<std::string>::iterator it = m_logs.begin(); it != m_logs.end(); ++it)
        ret += *it;
    return ret;
}
