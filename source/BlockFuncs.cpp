#include "stdinclude.h"
#include "Typedefs.h"
#include "Definitions.h"
#include "API.h"
#include "BlockFuncs.h"
#include "Blocks.h"
#include "World.h"

bool GrassBUF(Blocks::BUDDP * args)
{
    long long bx = args->cx;
    long long by = args->cy;
    long long bz = args->cz;
    block b = World::getblock(bx, by + 1, bz);
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

bool WaterBUF(Blocks::BUDDP * args)
{
    cout << "callwith:" << args->slf->Data81 << " " << args->slf->Data82 << endl;
    if (args->slf->Data81 == 1)
    {
        long long bx = args->cx;
        long long by = args->cy;
        long long bz = args->cz;
        block b = World::getblock(bx, by - 1, bz);
        if (b.ID == Blocks::AIR)
        {
            World::Modifyblock(bx, by - 1, bz, block(Blocks::WATER, 1, args->slf->Data82));
            return true;
        }
        else if (b.ID == Blocks::WATER)
        {
            int s = min(b.Data82 + args->slf->Data82, 255);
            if (s > b.Data82)
            {
                World::Modifyblock(bx, by - 1, bz, block(Blocks::WATER, 1, s));
                return true;
            }
            else
            {
                return false;
            }
        }
        if (args->slf->Data82 < 16)
            return false;

        bool set = false;
        const int vec[] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };
        for(int i = 0; i < 4; i++)
        {
            b = World::getblock(bx + vec[i][0], by, bz + vec[i][1]);
            cout << b.ID << " ";
            if (b.ID == Blocks::AIR)
            {
                World::Modifyblock(bx + vec[i][0], by, bz + vec[i][1], block(Blocks::WATER, 1, args->slf->Data82 - 16));
                set = true;
            }
            else if (b.ID == Blocks::WATER)
            {
                if (b.Data82 < args->slf->Data82 - 16)
                {
                    World::Modifyblock(bx + vec[i][0], by, bz + vec[i][1], block(Blocks::WATER, 1, args->slf->Data82 - 16));
                    set = true;
                }
            }
        }
        if (set)
            return true;
    }
    return false;
}
