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

#include "Definitions.h"
#include "BlockFuncs.h"
#include "Blocks.h"
#include "World.h"

bool GrassBUF(Blocks::BUDDP * args)
{
    int bx = args->cx, by = args->cy, bz = args->cz;
    block b;
    b = World::getblock(bx, by + 1, bz);
    if (b.ID != Blocks::AIR)
    {
        *(args->slf)=block(Blocks::DIRT);
        return true;
    }
    else
    {
        return false;
    }
}

bool WaterBUF(Blocks::BUDDP* args)
{
    if (args->slf->Data81==1)
    {
        int bx = args->cx, by = args->cy, bz = args->cz;
        bool set = false;
        block b;
        const int vec[][] = { { -1, 0, 0 }, { 1, 0, 0 }, { 0, -1, 0 }, { 0, 0, -1 }, { 0, 0, 1 } };
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
    else if (args->slf->Data81 == 0)
    {
        int bx = args->cx, by = args->cy, bz = args->cz;
        bool u = false;
        block b = World::getblock(bx, by - 1, bz);
        if (b.ID == Blocks::WATER && b.Data81 == 0 && b.Data82 != 255)
        {
            int s = b.Data82 + args->slf->Data82;
            if (s < 255)
            {
                World::setblock(bx, by - 1, bz, block(Blocks::WATER, 0, s));
                return true;
            }
            else
            {
                World::setblock(bx, by - 1, bz, block(Blocks::WATER, 0, 255));
                args->slf->Data82 = s - 255;
                u = true;
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
                    total += b.Data82;
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
    else
    {
        return false;
    }
}

bool LavaBUF(Blocks::BUDDP * args)
{
    if (args->slf->Data81 == 1)
    {
        int bx = args->cx;
        int by = args->cy;
        int bz = args->cz;
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
        int bx = args->cx;
        int by = args->cy;
        int bz = args->cz;
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
