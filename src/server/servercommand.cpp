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
#include <utils.h>
#include "settings.h"
#include <type.h>

bool inputThreadRunning = true;

CommandMap commandMap;
#define CommandDefine(commandName, commandAuthor, commandHelp) commandMap.insert({commandName, std::pair<CommandInfo,CommandHandleFunction>({commandAuthor, commandHelp},[](Command cmd)->CommandExecuteStat
#define EndCommandDefine )})

void initCommands()
{
    CommandDefine("help", "Internel", "Help")
    {
        std::string helpString = "\nAvailable commands:\n";
        for (const auto& command : commandMap)
        {
            helpString += command.first + " - " + command.second.first.author + " : " + command.second.first.help + "\n";
        }
        return{ true, helpString };
    }
    EndCommandDefine;

    CommandDefine("test.hello", "Internal", "Say hello")
    {
        return{ true, "Hello!" };
    }
    EndCommandDefine;

    CommandDefine("test.fatal", "Internal", "test fatal log.")
    {
        fatalstream << "execute test.fatal!";
        return { true , "" };
    }
    EndCommandDefine;

    CommandDefine("test.error", "Internal", "test error log.")
    {
        errorstream << "execute test.error!";
        return{ true , "" };
    }
    EndCommandDefine;

    CommandDefine("test.warning", "Internal", "do NOT use it :-)")
    {
        warningstream << "Your computer will explode in three seconds!!!";
        return{ true , "" };
    }
    EndCommandDefine;

    CommandDefine("test.rainbow", "Internal", "test color.")
    {
        infostream << "Grayscales:";
        infostream << CColor::black << "2333333333 [0%]";
        infostream << CColor::dgray << "2333333333 [50%]";
        infostream << CColor::gray << "2333333333 [75%]";
        infostream << CColor::white << "2333333333 [100%]";
        infostream << "Dark colors:";
        infostream << CColor::dred << "2333333333 [dark red]";
        infostream << CColor::dyellow << "2333333333 [dark yellow]";
        infostream << CColor::dgreen << "2333333333 [dark green]";
        infostream << CColor::dcyan << "2333333333 [dark cyan]";
        infostream << CColor::dblue << "2333333333 [dark blue]";
        infostream << CColor::dmagenta << "2333333333 [dark magenta]";
        infostream << "Bright colors:";
        infostream << CColor::red << "2333333333 [red]";
        infostream << CColor::yellow << "2333333333 [yellow]";
        infostream << CColor::green << "2333333333 [green]";
        infostream << CColor::cyan << "2333333333 [cyan]";
        infostream << CColor::blue << "2333333333 [blue]";
        infostream << CColor::magenta << "2333333333 [magenta]";
        return{ true , "" };
    }
    EndCommandDefine;

    CommandDefine("server.stop", "Internal", "Stop the server")
    {
        ioService.stop();
        inputThreadRunning = false;
        return{ true, "" };
    }
    EndCommandDefine;

    CommandDefine("conf.set", "Internal", "Set one configuration item. Usage: conf.set <confname> <value>")
    {
        if (cmd.args.size() == 2)
        {
            settings.set(cmd.args[0], string2type(cmd.args[1]));
            return{ true, "Set" };
        }
        else
        {
            return{ false, "Usage: conf.set <confname> <value>" };
        }
    }
    EndCommandDefine;

    CommandDefine("conf.get", "Internal", "Get one configuration item. Usage: conf.get <confname>")
    {
        if (cmd.args.size() == 1)
        {
            if(settings.have(cmd.args[0]))
                return{ true, cmd.args[0] + " = " + type2string(settings.get(cmd.args[0])) };
            else
                return{ false, "The configuration item does not exist." };
        }
        else
        {
            return{ false, "Usage: conf.get <confname>" };
        }
    }
    EndCommandDefine;

    CommandDefine("conf.show", "Internal", "Show the configuration.")
    {
        std::string conf;
        for (const auto& p : settings.getMap())
        {
            conf += '\n' + p.first + " = " + type2string(p.second);
        }
        return{ true,conf };
    }
    EndCommandDefine;

    CommandDefine("conf.save", "Internal", "Save the configuration.")
    {
        settings.save();
        return{ true,"Done!" };
    }
    EndCommandDefine;
}

CommandExecuteStat handleCommand(Command cmd)
{
    strtolower(cmd.name);
    auto result = commandMap.find(cmd.name);
    if (result != commandMap.end())
        return (*result).second.second(cmd);
    else
        return{ false,"Failed to execute the command: The command does not exist, type help for available commands." };
}

void inputThreadFunc()
{
    initCommands();
    while (inputThreadRunning)
    {
        using namespace std::chrono_literals;
        std::string input;
        std::cout << CColor::gray.get() << "> ";
        std::getline(std::cin, input);
        auto result = handleCommand(Command(input));
        if (result.info != "") infostream << result.info;
    }
}
