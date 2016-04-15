#include "Definitions.h"
#include "Blocks.h"
#include "World.h"

class Air :public Blocks::SingleBlock
{
public:
    Air() :SingleBlock("NEWorld.Blocks.Air", false, false, false, false, 0) {}
};

class Rock :public Blocks::SingleBlock
{
public:
    Rock() :SingleBlock("NEWorld.Blocks.Rock", true, true, false, false, 2) {}
};

class Grass :public Blocks::SingleBlock
{
public:
    Grass() :SingleBlock("NEWorld.Blocks.Grass", true, true, false, false, 5) {}
    bool ExecBUF(Blocks::BUDDP * args)
    {
        long long bx = args->cx, by = args->cy, bz = args->cz;
        block b;
        b = World::getblock(bx, by + 1, bz);
        if (b.ID != Blocks::AIR)
        {
            *(args->slf) = block(Blocks::DIRT);
            return true;
        }
        else
        {
            return false;
        }
    }
};

class Dirt :public Blocks::SingleBlock
{
public:
    Dirt() :SingleBlock("NEWorld.Blocks.Dirt", true, true, false, false, 5) {}
};

class Stone :public Blocks::SingleBlock
{
public:
    Stone() : SingleBlock("NEWorld.Blocks.Stone", true, true, false, false, 2) {}
};

class Plank :public Blocks::SingleBlock
{
public:
    Plank() : SingleBlock("NEWorld.Blocks.Plank", true, true, false, false, 5) {}
};

class Wood :public Blocks::SingleBlock
{
public:
    Wood() : SingleBlock("NEWorld.Blocks.Wood", true, true, false, false, 5) {}
};

class BedRock :public Blocks::SingleBlock
{
public:
    BedRock() : SingleBlock("NEWorld.Blocks.Bedrock", true, true, false, false, 0) {}
};

class Leaf :public Blocks::SingleBlock
{
public:
    Leaf() : SingleBlock("NEWorld.Blocks.Leaf", true, false, false, false, 15) {}
};

class Glass :public Blocks::SingleBlock
{
public:
    Glass() : SingleBlock("NEWorld.Blocks.Glass", true, false, false, false, 30) {}
};

class Water :public Blocks::SingleBlock
{
public:
    Water() : SingleBlock("NEWorld.Blocks.Water", false, false, true, false, 0) {}
    bool ExecBUF(Blocks::BUDDP* args)
    {
        if (args->slf->Data81 == 1)
        {
            long long bx = args->cx, by = args->cy, bz = args->cz;
            bool set = false;
            block b;
            const int vec[5][3] = { { -1, 0, 0 },{ 1, 0, 0 },{ 0, -1, 0 },{ 0, 0, -1 },{ 0, 0, 1 } };
            for (int i = 0; i < 5; i++)
            {
                b = World::getblock(bx + vec[i][0], by + vec[i][1], bz + vec[i][2]);
                if (b.ID == Blocks::AIR || ((b.ID == Blocks::WATER) && (b.Data81 == 0)))
                {
                    World::setblock(bx + vec[i][0], by + vec[i][1], bz + vec[i][2], block(Blocks::WATER, 1, 255));
                    set = true;
                }
            }
            if (set) return true;
        }
        else if (!args->slf->Data81)
        {
            long long bx = args->cx, by = args->cy, bz = args->cz;
            bool u = false;
            block b = World::getblock(bx, by - 1, bz);
            if (b.ID == Blocks::WATER && b.Data81 == 0 && b.Data82 != 255)
            {
                int s = b.Data82 + args->slf->Data82;
                if (s < 255)
                {
                    World::setblock(bx, by - 1, bz, block(Blocks::WATER, 0, s));
                    *args->slf = block(Blocks::AIR);
                }
                else
                {
                    World::setblock(bx, by - 1, bz, block(Blocks::WATER, 0, 255));
                    args->slf->Data82 = s - 255;
                }
                return true;
            }
            else if (b.ID == Blocks::AIR)
            {
                World::setblock(bx, by - 1, bz, block(Blocks::WATER, 0, args->slf->Data82));
                *args->slf = block(Blocks::AIR);
                return true;
            }

            if (args->slf->Data82 > 16)
            {
                const int vec[4][3] = { { -1, 0, 0 },{ 1, 0, 0 },{ 0, 0, -1 },{ 0, 0, 1 } };
                bool pos[4] = { false, false, false, false };
                int total = args->slf->Data82, bcount = 1;
                for (int i = 0; i < 4; i++)
                {
                    b = World::getblock(bx + vec[i][0], by + vec[i][1], bz + vec[i][2]);
                    if ((b.ID == Blocks::WATER) && (b.Data81 == 0) && (b.Data82 < (args->slf->Data82 - 2)))
                    {
                        pos[i] = u = true;
                        total = total + b.Data82;
                        bcount++;
                    }
                    else if (b.ID == Blocks::AIR)
                    {
                        pos[i] = u = true;
                        bcount++;
                    }
                }
                if (pos[0] || pos[1] || pos[2] || pos[3])
                {
                    total /= bcount;
                    args->slf->Data82 = total;
                    for (int i = 0; i < 4; i++)
                        if (pos[i]) World::setblock(bx + vec[i][0], by + vec[i][1], bz + vec[i][2], block(Blocks::WATER, 0, total));
                }
                if (u) return u;
            }
        }
        return false;
    }
};

class Lava :public Blocks::SingleBlock
{
public:
    Lava() : SingleBlock("NEWorld.Blocks.Lava", false, false, true, false, 0) {}

    bool ExecBUF(Blocks::BUDDP * args)
    {
        if (args->slf->Data81 == 1)
        {
            long long bx = args->cx;
            long long by = args->cy;
            long long bz = args->cz;
            bool set = false;
            block b;
            const int vec[5][3] = { { -1, 0, 0 },{ 1, 0, 0 },{ 0, -1, 0 },{ 0, 0, -1 },{ 0, 0, 1 } };
            for (int i = 0; i < 5; i++)
            {
                b = World::getblock(bx + vec[i][0], by + vec[i][1], bz + vec[i][2]);
                if (b.ID == Blocks::AIR || ((b.ID == Blocks::LAVA) && (b.Data81 == 0)))
                {
                    World::setblock(bx + vec[i][0], by + vec[i][1], bz + vec[i][2], block(Blocks::LAVA, 1, 255));
                    set = true;
                }
            }
            if (set) return true;
        }
        else if (args->slf->Data81 == 0)
        {
            long long bx = args->cx;
            long long by = args->cy;
            long long bz = args->cz;
            bool u = false;
            block b = World::getblock(bx, by - 1, bz);
            if (b.ID == Blocks::LAVA && b.Data81 == 0 && b.Data82 != 255)
            {
                int s = b.Data82 + args->slf->Data82;
                if (s < 255)
                {
                    World::setblock(bx, by - 1, bz, block(Blocks::LAVA, 0, s));
                    *args->slf = block(Blocks::AIR);
                }
                else
                {
                    World::setblock(bx, by - 1, bz, block(Blocks::LAVA, 0, 255));
                    args->slf->Data82 = s - 255;
                }
                return true;
            }
            else if (b.ID == Blocks::AIR)
            {
                World::setblock(bx, by - 1, bz, block(Blocks::LAVA, 0, args->slf->Data82));
                *args->slf = block(Blocks::AIR);
                return true;
            }

            if (args->slf->Data82 > 16)
            {
                const int vec[4][3] = { { -1, 0, 0 },{ 1, 0, 0 },{ 0, 0, -1 },{ 0, 0, 1 } };
                bool pos[4] = { false, false, false, false };
                int total = args->slf->Data82, bcount = 1;
                for (int i = 0; i < 4; i++)
                {
                    b = World::getblock(bx + vec[i][0], by + vec[i][1], bz + vec[i][2]);
                    if ((b.ID == Blocks::LAVA) && (b.Data81 == 0) && (b.Data82 < (args->slf->Data82 - 2)))
                    {
                        pos[i] = u = true;
                        total = total + b.Data82;
                        bcount++;
                    }
                    else if (b.ID == Blocks::AIR)
                    {
                        pos[i] = u = true;
                        bcount++;
                    }
                }
                if (pos[0] || pos[1] || pos[2] || pos[3])
                {
                    total /= bcount;
                    args->slf->Data82 = total;
                    for (int i = 0; i < 4; i++)
                        if (pos[i]) World::setblock(bx + vec[i][0], by + vec[i][1], bz + vec[i][2], block(Blocks::LAVA, 0, total));
                }
                if (u) return u;
            }
        }
        return false;
    }

};

class GlowStone :public Blocks::SingleBlock
{
public:
    GlowStone() : SingleBlock("NEWorld.Blocks.GlowStone", true, true, false, false, 10) {}
};

class Sand :public Blocks::SingleBlock
{
public:
    Sand() : SingleBlock("NEWorld.Blocks.Sand", true, true, false, false, 8) {}
};

class Cement :public Blocks::SingleBlock
{
public:
    Cement() : SingleBlock("NEWorld.Blocks.Cement", true, true, false, false, 0.5f) {}
};

class Ice :public Blocks::SingleBlock
{
public:
    Ice() : SingleBlock("NEWorld.Blocks.Ice", true, false, true, false, 25) {}
};

class Coal_B :public Blocks::SingleBlock
{
public:
    Coal_B() : SingleBlock("NEWorld.Blocks.Coal Block", true, true, false, false, 1) {}
};

class Iron_B :public Blocks::SingleBlock
{
public:
    Iron_B() : SingleBlock("NEWorld.Blocks.Iron Block", true, true, false, false, 0.5f) {}
};

class TNT :public Blocks::SingleBlock
{
public:
    TNT() : SingleBlock("NEWorld.Blocks.TNT", true, true, false, true, 25) {}
};

class Null_B :public Blocks::SingleBlock
{
public:
    Null_B() : SingleBlock("NEWorld.Blocks.Null Block", true, true, false, false, 0) {}
};

void* Allocator(string key)
{
    if (key == "NEWorld.Blocks.Air") return new Air;
    if (key == "NEWorld.Blocks.Rock") return new Rock;
    if (key == "NEWorld.Blocks.Grass") return new Grass;
    if (key == "NEWorld.Blocks.Dirt") return new Dirt;
    if (key == "NEWorld.Blocks.Stone") return new Stone;
    if (key == "NEWorld.Blocks.Plank") return new Plank;
    if (key == "NEWorld.Blocks.Wood") return new Wood;
    if (key == "NEWorld.Blocks.Bedrock") return new BedRock;
    if (key == "NEWorld.Blocks.Leaf") return new Leaf;
    if (key == "NEWorld.Blocks.Glass") return new Glass;
    if (key == "NEWorld.Blocks.Water") return new Water;
    if (key == "NEWorld.Blocks.Lava") return new Lava;
    if (key == "NEWorld.Blocks.GlowStone") return new GlowStone;
    if (key == "NEWorld.Blocks.Sand") return new Sand;
    if (key == "NEWorld.Blocks.Cement") return new Cement;
    if (key == "NEWorld.Blocks.Ice") return new Ice;
    if (key == "NEWorld.Blocks.Coal Block") return new Coal_B;
    if (key == "NEWorld.Blocks.Iron Block") return new Iron_B;
    if (key == "NEWorld.Blocks.TNT") return new TNT;
    if (key == "NEWorld.Blocks.Null Block") return new Null_B;
}
