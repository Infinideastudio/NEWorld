/*
 * NEWorld: An free game with similar rules to Minecraft.
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

#ifndef PLAYER_H
#define PLAYER_H

#include "Definitions.h"

const double g = 9.8;
const double EDrop = 0.1;
const double speedCast = 1 / 20.0;

struct PlayerPacket;
namespace Hitbox
{
struct AABB;
}
class Frustum;

class Player
{
public:
    static void init();
    static void spawn();

    static void updatePosition();

    static bool save(string worldn);
    static bool load(string worldn);

    static bool AddItem(item itemname, short amount = 1);
    static bool putBlock(int x, int y, int z, block blockname);

    static void changeGameMode(int gamemode);

    static PlayerPacket convertToPlayerPacket();
    static Hitbox::AABB playerbox;
    static vector<Hitbox::AABB> Hitboxes;
    static double xa, ya, za, xd, yd, zd;
    static double health, healthMax, healSpeed, dropDamage;
    static onlineid onlineID;
    static string name;
    static Frustum ViewFrustum;

    enum GameMode { Survival, Creative };
    static int gamemode;
    static bool Glide;
    static bool Flying;
    static bool CrossWall;
    static double glidingMinimumSpeed;

    static bool OnGround;
    static bool Running;
    static bool NearWall;
    static bool inWater;
    static bool glidingNow;

    static double speed;
    static int AirJumps;
    static int cxt, cyt, czt, cxtl, cytl, cztl;
    static double lookupdown, heading, xpos, ypos, zpos, jump;
    static double xlookspeed, ylookspeed;

    static float height;
    static float heightExt;

    static item BlockInHand;
    static ubyte indexInHand;
    static item inventory[4][10];
    static short inventoryAmount[4][10];

    static double glidingEnergy, glidingSpeed;

};

#endif
