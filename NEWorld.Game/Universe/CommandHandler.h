#pragma once

#include <utility>
#include <Renderer/World/ShadowMaps.h>
#include "Command.h"
#include "GameView.h"
#include "Universe/World/Blocks.h"
#include "Textures.h"
#include "Renderer/Renderer.h"
#include "TextRenderer.h"
#include "Player.h"
#include "Universe/World/World.h"
#include "Renderer/World/WorldRenderer.h"
#include "Particles.h"
#include "Hitbox.h"
#include "GUI.h"
#include "Menus.h"
#include "Items.h"
#include "Setup.h"
#include "AudioSystem.h"
#include "Universe/Game.h"

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
        commands.emplace_back("/give", [](const std::vector<std::string> &command) {
            if (command.size() != 3) return false;
            item itemid;
            conv(command[1], itemid);
            short amount;
            conv(command[2], amount);
            Player::addItem(itemid, amount);
            return true;
        });
        commands.emplace_back("/tp", [](const std::vector<std::string> &command) {
            if (command.size() != 4) return false;
			Double3 targetPos;
            conv(command[1], targetPos.X);
            conv(command[2], targetPos.Y);
            conv(command[3], targetPos.Z);
			Player::Pos = targetPos;
            return true;
        });
        commands.emplace_back("/suicide", [](const std::vector<std::string> &command) {
            if (command.size() != 1) return false;
            Player::spawn();
            return true;
        });
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
        commands.emplace_back("/gamemode", [](const std::vector<std::string> &command) {
            if (command.size() != 2) return false;
            int mode;
            conv(command[1], mode);
            Player::changeGameMode(mode);
            return true;
        });
        commands.emplace_back("/kit", [](const std::vector<std::string> &command) {
            if (command.size() != 1) return false;
            Player::inventory[0][0] = 1;
            Player::inventoryAmount[0][0] = 255;
            Player::inventory[0][1] = 2;
            Player::inventoryAmount[0][1] = 255;
            Player::inventory[0][2] = 3;
            Player::inventoryAmount[0][2] = 255;
            Player::inventory[0][3] = 4;
            Player::inventoryAmount[0][3] = 255;
            Player::inventory[0][4] = 5;
            Player::inventoryAmount[0][4] = 255;
            Player::inventory[0][5] = 6;
            Player::inventoryAmount[0][5] = 255;
            Player::inventory[0][6] = 7;
            Player::inventoryAmount[0][6] = 255;
            Player::inventory[0][7] = 8;
            Player::inventoryAmount[0][7] = 255;
            Player::inventory[0][8] = 9;
            Player::inventoryAmount[0][8] = 255;
            Player::inventory[0][9] = 10;
            Player::inventoryAmount[0][9] = 255;
            Player::inventory[1][0] = 11;
            Player::inventoryAmount[1][0] = 255;
            Player::inventory[1][1] = 12;
            Player::inventoryAmount[1][1] = 255;
            Player::inventory[1][2] = 13;
            Player::inventoryAmount[1][2] = 255;
            Player::inventory[1][3] = 14;
            Player::inventoryAmount[1][3] = 255;
            Player::inventory[1][4] = 15;
            Player::inventoryAmount[1][4] = 255;
            Player::inventory[1][5] = 16;
            Player::inventoryAmount[1][5] = 255;
            Player::inventory[1][6] = 17;
            Player::inventoryAmount[1][6] = 255;
            Player::inventory[1][7] = 18;
            Player::inventoryAmount[1][7] = 255;
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
