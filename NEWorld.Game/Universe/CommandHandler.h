#pragma once

#include "Command.h"
#include "Universe/World/Blocks.h"
#include "Player.h"
#include "Universe/World/World.h"
#include "Items.h"

std::vector<Command> commands;

class CommandHandler {

public:
    static bool doCommand(const std::vector<std::string> &command) {
        for (unsigned int i = 0; i != commands.size(); i++) {
            if (command[0] == commands[i].identifier) {
                return commands[i].execute(command);
            }
        }
        return false;
    }

    static void registerCommands() {
        commands.emplace_back("/setblock", [](const std::vector<std::string> &command) {
            if (command.size() != 5) return false;
            int x;
            conv(command[1], x);
            int y;
            conv(command[2], y);
            int z;
            conv(command[3], z);
            Block b;
            conv(command[4], b);
            World::SetBlock({(x), (y), (z)}, b);
            return true;
        });
        commands.emplace_back("/tree", [](const std::vector<std::string> &command) {
            if (command.size() != 4) return false;
            int x;
            conv(command[1], x);
            int y;
            conv(command[2], y);
            int z;
            conv(command[3], z);
            World::buildtree({x, y, z});
            return true;
        });
        commands.emplace_back("/explode", [](const std::vector<std::string> &command) {
            if (command.size() != 5) return false;
            int x;
            conv(command[1], x);
            int y;
            conv(command[2], y);
            int z;
            conv(command[3], z);
            int r;
            conv(command[4], r);
            World::explode(x, y, z, r);
            return true;
        });
        commands.emplace_back("/time", [](const std::vector<std::string> &command) {
            if (command.size() != 2) return false;
            int time;
            conv(command[1], time);
            if (time < 0 || time > gameTimeMax) return false;
            gametime = time;
            return true;
        });
    }
};
