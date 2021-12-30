#pragma once

#include "Command.h"
#include "Universe/World/Blocks.h"
#include "Universe/World/World.h"
#include "Items.h"

std::vector<Command> commands;

class CommandHandler {

public:
    static bool doCommand(const std::vector<std::string> &command) {
        for (auto &i: commands) {
            if (command[0] == i.identifier) return i.execute(command);
        }
        return false;
    }

    static void registerCommands() {
        commands.emplace_back("/setblock", [](const std::vector<std::string> &command) {
            if (command.size() != 5) return false;
            const auto x = std::stoi(command[1]);
            const auto y = std::stoi(command[2]);
            const auto z = std::stoi(command[3]);
            const Block b = std::stoll(command[4]);
            World::SetBlock({x, y, z}, b);
            return true;
        });
        commands.emplace_back("/tree", [](const std::vector<std::string> &command) {
            if (command.size() != 4) return false;
            const auto x = std::stoi(command[1]);
            const auto y = std::stoi(command[2]);
            const auto z = std::stoi(command[3]);
            World::buildtree({x, y, z});
            return true;
        });
        commands.emplace_back("/explode", [](const std::vector<std::string> &command) {
            if (command.size() != 5) return false;
            const auto x = std::stoi(command[1]);
            const auto y = std::stoi(command[2]);
            const auto z = std::stoi(command[3]);
            const auto r = std::stoi(command[4]);
            World::explode(x, y, z, r);
            return true;
        });
        commands.emplace_back("/time", [](const std::vector<std::string> &command) {
            if (command.size() != 2) return false;
            const auto time = std::stoi(command[4]);
            if (time < 0 || time > gameTimeMax) return false;
            gametime = time;
            return true;
        });
    }
};
