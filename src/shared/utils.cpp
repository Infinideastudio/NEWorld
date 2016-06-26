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

#include "utils.h"
#include <algorithm>

std::vector<std::string> split(const std::string& src, std::string separate_character)
{
    std::vector<std::string> strs;
    int separate_characterLen = separate_character.size();
    int last_position = 0, index = -1;
    while (-1 != (index = src.find(separate_character, last_position)))
    {
        strs.push_back(src.substr(last_position, index - last_position));
        last_position = index + separate_characterLen;
    }
    std::string lastString = src.substr(last_position);
    if (!lastString.empty())
        strs.push_back(lastString);
    return strs;
};

void trim(std::string& s)
{
    if (s.empty())
        return;

    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
}

void strtolower(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(), tolower);
}
