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
#include <map>

struct CommandExecuteStat
{
    bool success;
    std::string info;
};

class Command
{
public:
    explicit Command(std::string rawString)
    {
        auto split = [](const std::string& src, std::string separate_character)->std::vector<std::string>
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
        args = split(rawString, " ");
        name = args[0];
        args.erase(args.begin());
    }
    std::string name;
    std::vector<std::string> args;
};

struct CommandInfo
{
    std::string author;
    std::string help;
};

using CommandHandleFunction = std::function<CommandExecuteStat(Command)>;
using CommandMap = std::map<std::string, std::pair<CommandInfo, CommandHandleFunction>>;

#endif // !COMMAND_H_
