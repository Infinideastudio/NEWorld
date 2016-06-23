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
#include "precomp.h"
#include "settingsmanager.h"

bool isDecimal(std::string str)
{
    if (str.empty()) return false;
    bool ret = true, dot = false;
    for (char c : str)
    {
        if (c == '.' && !dot) dot = true;
        else if (c<'0' || c>'9') ret = false;
    }
    return ret;
}

bool isInteger(std::string str)
{
    if (str.empty()) return false;
    bool ret = true;
    for (char c : str)
    {
        if (c<'0' || c>'9') ret = false;
    }
    return ret;
}

bool isBoolean(std::string str)
{
    if (str.empty()) return false;
    std::transform(str.begin(), str.end(), str.begin(), tolower);
    return str == "true" || str == "false";
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
    std::transform(str.begin(), str.end(), str.begin(), tolower);
    return str == "true";
}

Settings::SettingsMap Settings::readFromFile(std::ifstream & file)
{
    Settings::SettingsMap map;
    if (file)
    {
        while (!file.eof())
        {
            std::string key, valueStr;
            file >> key >> valueStr;
            boost::spirit::hold_any value;
            if (isBoolean(valueStr)) value = getBoolean(valueStr);
            else if (isInteger(valueStr)) value = getInteger(valueStr);
            else if (isDecimal(valueStr)) value = getDecimal(valueStr);
            map[key] = value;
        }
    }
    return map;
}

void Settings::writeToFile(std::ofstream & file, const Settings::SettingsMap& settings)
{
    for (const auto& p : settings)
    {
        file << p.first << " " << p.second << std::endl;
    }
}
