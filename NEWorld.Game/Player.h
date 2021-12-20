#pragma once

#include "Definitions.h"
#include "Math/Vector3.h"

const double g = 9.8;
const double EDrop = 0.1;
const double speedCast = 1 / 20.0;
/*
struct PlayerPacket;
namespace Hitbox { struct AABB; }
class Frustum;

class Player {
public:
    static void updatePosition(Double3 velocity);

    static bool save(std::string worldn);

    static bool load(std::string worldn);

    static bool addItem(item itemname, short amount = 1);

    static bool putBlock(int x, int y, int z, Block blockname);

    static void changeGameMode(int gamemode);

    static PlayerPacket convertToPlayerPacket();

    static Hitbox::AABB playerbox;
    static std::vector<Hitbox::AABB> Hitboxes;
    static double xa, ya, za, xd, yd, zd;
    static double health, healthMax, healSpeed, dropDamage;
    static onlineid onlineID;
    static std::string name;
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
	static Double3 Pos;
    static double lookupdown, heading, jump;
    static double xlookspeed, ylookspeed;

    static float height;
    static float heightExt;

    static item BlockInHand;
    static ubyte indexInHand;

    static constexpr int MaxStack = 255;
    static item inventory[4][10];
    static short inventoryAmount[4][10];

    static double glidingEnergy, glidingSpeed;

};*/