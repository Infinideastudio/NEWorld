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
#include "utils.h"

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
    strtolower(str);
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
    strtolower(str);
    return str == "true";
}

boost::spirit::hold_any string2type(std::string str)
{
    boost::spirit::hold_any value;
    if (isBoolean(str)) value = getBoolean(str);
    else if (isInteger(str)) value = getInteger(str);
    else if (isDecimal(str)) value = getDecimal(str);
    else warningstream << "Failed to read value:" << str << ", which maybe not supported.";
    return value;
}

std::string type2string(boost::spirit::hold_any var)
{
    if (var.type() == typeid(bool))
    {
        return var.cast<bool>() ? "true" : "false";
    }
    else if(var.type()==typeid(int))
    {
        return std::to_string(var.cast<int>());
    }
    else if (var.type() == typeid(double))
    {
        return std::to_string(var.cast<double>());
    }
    else if (var.type() == typeid(std::string))
    {
        return var.cast<std::string>();
    }
    else warningstream << "Failed to handle type " << var.type().name() << " which maybe not supported.";
    return "";
}

Settings::SettingsMap Settings::readFromFile(std::ifstream & file)
{
    Settings::SettingsMap map;
    if (file)
    {
        while (!file.eof())
        {
            std::string line;
            std::getline(file, line);
            auto vk = split(line, "=");
            if (vk.size() != 2) continue;
            std::string key = vk[0], value = vk[1];
            trim(key);
            strtolower(key);
            trim(value);
            map[key] = string2type(value);
        }
    }
    return map;
}

void Settings::writeToFile(std::ofstream & file, const Settings::SettingsMap& settings,bool minimal)
{
    for (const auto& p : settings)
    {
        if (minimal)
            file << p.first << "=" << type2string(p.second) << std::endl;
        else
            file << p.first << " = " << type2string(p.second) << std::endl;
    }
}
