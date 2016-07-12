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

#include <sstream>
#include "logger.h"
#include "type.h"
#include "utils.h"

boost::spirit::hold_any string2type(std::string str)
{
    boost::spirit::hold_any value;
    if (isBoolean(str)) value = getBoolean(str);
    else if (isInteger(str)) value = getInteger(str);
    else if (isDecimal(str)) value = getDecimal(str);
    else if (isString(str)) value = getString(str);
    else warningstream << "Failed to read value:" << str << ", which maybe not supported.";
    return value;
}

std::string type2string(boost::spirit::hold_any var)
{
    if (var.type() == typeid(bool))
    {
        return var.cast<bool>() ? "true" : "false";
    }
    if (var.type() == typeid(int))
    {
        return std::to_string(var.cast<int>());
    }
    if (var.type() == typeid(double))
    {
        return std::to_string(var.cast<double>());
    }
    if (var.type() == typeid(std::string))
    {
        return var.cast<std::string>();
    }
    warningstream << "Failed to handle type " << var.type().name() << " which maybe not supported.";
    return "";
}
