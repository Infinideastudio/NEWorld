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
#include <boost/spirit/home/support/detail/hold_any.hpp>

bool isDecimal(std::string str);
bool isInteger(std::string str);
bool isBoolean(std::string str);

double getDecimal(std::string str);
int getInteger(std::string str);
bool getBoolean(std::string str);

boost::spirit::hold_any string2type(std::string str);
std::string type2string(boost::spirit::hold_any var);



#endif