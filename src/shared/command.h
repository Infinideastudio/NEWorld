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

#ifndef COMMAND_H_
#define COMMAND_H_

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include "utils.h"

class CommandExecuteStat
{
public:
    CommandExecuteStat(bool s, std::string i) : success(s), info(i)
    {
    }

    bool success;
    std::string info;
};

class Command
{
public:
    explicit Command(std::string rawString)
    {
        args = split(rawString, " ");
        name = args.size() != 0 ? args[0] : "";
        if (args.size() != 0) args.erase(args.begin());
    }

    std::string name;
    std::vector<std::string> args;
};

class CommandInfo
{
public:
    CommandInfo(std::string a, std::string h) : author(a), help(h)
    {
    }

    std::string author;
    std::string help;
};

using CommandHandleFunction = std::function<CommandExecuteStat(Command)>;
using CommandMap = std::unordered_map<std::string, std::pair<CommandInfo, CommandHandleFunction>>;

#endif // !COMMAND_H_
