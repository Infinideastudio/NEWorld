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
#ifndef _TYPE_H__
#define _TYPE_H__

#include <string>
#include "utils.h"
#include <boost/spirit/home/support/detail/hold_any.hpp>

inline bool isDecimal(std::string str)
{
    if (str.empty()) return false;
    bool ret = true, dot = false, pos = true, beg = true;
    for (char c : str)
    {
        if (c == '.' && !dot) dot = true;
        else if (c == '-'&& beg) pos = false;
        else if ((c < '0' || c > '9') && (!beg&&c != '+'&&c != '-')) ret = false;
        if (beg) beg = false;
        if (!ret) break;
    }
    return ret;
}

inline bool isInteger(std::string str)
{
    if (str.empty()) return false;
    bool ret = true, beg = true, pos = true;
    for (char c : str)
    {
        if (c == '-'&& beg) pos = false;
        else if ((c < '0' || c > '9')&&(!beg&&c!='+'&&c!='-')) ret = false;
        if (beg) beg = false;
    }
    return ret;
}

inline bool isBoolean(std::string str)
{
    int getInteger(std::string str);
    if (str.empty()) return false;
    strtolower(str);
    int num = -1;
    if (isInteger(str)) num = getInteger(str);
    return str == "true" || str == "false" || num == 0 || num == 1;
}

inline bool isString(std::string str) //is something like "xxx"
{
    if (str.size() < 2) return false;
    if (str[0] != '"' || str[str.size() - 1] != '"') return false;
    return true;
}

inline double getDecimal(std::string str)
{
    return std::stod(str);
}

inline int getInteger(std::string str)
{
    return std::stoi(str);
}

inline bool getBoolean(std::string str)
{
    strtolower(str);
    return str == "true" || (isInteger(str) && getInteger(str) == 1);
}

inline std::string getString(std::string str)
{
    return str.substr(1, str.size() - 2);
}

boost::spirit::hold_any string2type(std::string str);
std::string type2string(boost::spirit::hold_any var);


#endif
