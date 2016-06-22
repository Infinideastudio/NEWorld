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
#include <atomic>
#include "server.h"

bool inputThreadRunning = true;

CommandMap commandMap;
#define CommandDefine(commandName, commandAuthor, commandHelp) commandMap.insert({commandName, std::pair<CommandInfo,CommandHandleFunction>({commandAuthor, commandHelp},[](Command)->CommandExecuteStat
#define EndCommandDefine )})

void initCommands()
{
    CommandDefine("help", "Internel", "帮助")
    {
        std::string helpString = "\nAvailable commands:\n";
        for (const auto& command : commandMap)
        {
            helpString += command.first + " - " + command.second.first.author + " : " + command.second.first.help + "\n";
        }
        return{ true, helpString };
    }
    EndCommandDefine;

    CommandDefine("hello", "Internal", "Say hello")
    {
        return{ true, "Hello!" };
    }
    EndCommandDefine;

    CommandDefine("stop", "Internal", "Stop the server")
    {
        ioService.stop();
        inputThreadRunning = false;
        return{ true, "" };
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
        return{ false,"Failed to execute the command: command is not found, type help for available commands." };
}

void inputThreadFunc()
{
    initCommands();
    while (inputThreadRunning)
    {
        using namespace std::chrono_literals;
        std::string input;
        std::this_thread::sleep_for(100ms);
        std::cout << "> ";
        std::getline(std::cin, input);
        auto result = handleCommand(Command(input));
        if (result.info != "") infostream << result.info;
    }
}
