// 
// CLI: command.hpp
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#pragma once
#include <string>
#include <atomic>
#include <iostream>
#include "Common/Logger.h"
#include "Common/StringUtils.h"
#include <unordered_map>
#include <thread>
#include <functional>

class CommandExecuteStat {
public:
    CommandExecuteStat(bool s, std::string i) : success(s), info(i) { }

    bool success;
    std::string info;
};

class Command {
public:
    explicit Command(std::string rawString) {
        args = split(rawString, ' ');
        name = args.size() != 0 ? args[0] : "";
        if (args.size() != 0) args.erase(args.begin());
    }

    std::string name;
    std::vector<std::string> args;
};

class CommandInfo {
public:
    CommandInfo(std::string a, std::string h) : author(a), help(h) { }

    std::string author;
    std::string help;
};

using CommandHandleFunction = std::function<CommandExecuteStat(Command)>;
using CommandMap = std::unordered_map<std::string, std::pair<CommandInfo, CommandHandleFunction>>;


class CommandManager {
public:
    CommandManager() :
        mMainloop([this] { inputLoop(); }) { }

    ~CommandManager() {
        mThreadRunning.store(false, std::memory_order_release);
        if (!mWaitingForInput.load(std::memory_order_acquire)) {
            mMainloop.join();
            debugstream << "Input thread exited.";
        }
        else {
            mMainloop.detach();
            debugstream << "Input thread detached.";
        }
    }

    CommandManager(const CommandManager&) = delete;
    CommandManager& operator=(const CommandManager&) = delete;

    CommandMap& getCommandMap() { return mCommandMap; }

    void inputLoop() {
        while (mThreadRunning.load(std::memory_order_acquire)) {
            std::string input;
            //std::cout << LColorFunc::white << "$> " << LColorFunc::lwhite;
            mWaitingForInput.store(true, std::memory_order_release);
            getline(std::cin, input);
            mWaitingForInput.store(false, std::memory_order_release);
            auto result = handleCommand(Command(input));
            if (result.info != "")
                infostream << result.info;
        }
    }

    void setRunningStatus(bool s) { mThreadRunning = s; }

    void registerCommand(std::string name, CommandInfo info, CommandHandleFunction func) {
        mCommandMap.insert({name, {info, func}});
    }

private:
    CommandExecuteStat handleCommand(Command cmd) {
        strToLower(cmd.name);
        auto result = mCommandMap.find(cmd.name);
        if (result != mCommandMap.end())
            return (*result).second.second(cmd);
        return {false, "Command not exists, type help for available commands."};
    }

    std::thread mMainloop;
    std::atomic_bool mThreadRunning{true};
    std::atomic_bool mWaitingForInput{false};
    CommandMap mCommandMap;
};
