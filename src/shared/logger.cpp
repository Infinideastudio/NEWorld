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

#include "Definitions.h"
#include "logger.h"
#include <assert.h>

void Logger::log(std::string information, CriticalLevel level)
{
    std::stringstream ss;
    ss << '[' << clock() * 1.0 / CLOCKS_PER_SEC << "] ";
    switch(level)
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
        //为了规避“三字符转义序列”(trigraph convert)
        //不得出现"??)"三个连续的字符
        ss << "( ?) ";
    }
    ss << information << endl;
    m_logs.push_back(ss.str());
#ifdef NEWORLD_TARGET_MACOSX
    //(个人习惯)在Mac上将信息即时显示在标准输出上。
    cout << ss.str() << endl;
#endif
}

string Logger::exportToString()
{
    std::string ret;
    for (vector<string>::iterator it = m_logs.begin(); it != m_logs.end(); ++it)
        ret += *it;
    return ret;
}


