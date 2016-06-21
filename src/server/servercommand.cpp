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
#include "servercommand.h"
#include <logger.h>
#include <thread>
#include <chrono>
#include <command.h>
#include <algorithm>

CommandMap commandMap;
#define CommandDefine(commandName, commandAuthor, commandHelp) commandMap.insert({commandName, std::pair<CommandInfo,CommandHandleFunction>({commandAuthor, commandHelp},[](Command)->CommandExecuteStat
#define EndCommandDefine )})
void initCommands()
{
    CommandDefine("hi", "Internal", "打招呼")
    {
        return{ true, "Hi!" };
    }
    EndCommandDefine;

    CommandDefine("help", "Internel", "帮助")
    {
        std::string helpString = "\n可用的指令有:\n";
        for (const auto& command : commandMap)
        {
            helpString += command.first + " - " + command.second.first.author + " : " + command.second.first.help + "\n";
        }
        return{ true, helpString };
    }
    EndCommandDefine;
}

CommandExecuteStat handleCommand(Command cmd)
{
    std::transform(cmd.name.begin(), cmd.name.end(), cmd.name.begin(), tolower);
    auto result = commandMap.find(cmd.name);
    if (result != commandMap.end())
        return (*result).second.second(cmd);
    else
        return{ false,"指令执行失败: 找不到该指令!请输入help查看帮助" };
}

void inputThreadFunc()
{
    initCommands();
    while (true)
    {
        using namespace std::chrono_literals;
        std::string input;
        std::this_thread::sleep_for(100ms);
        std::cout << "> ";
        std::getline(std::cin, input);
        infostream << handleCommand(Command(input)).info;
    }
}
