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

#include "pluginapi.h"

World* world;
BlockManager* blocks;

// Pointers to NEWorld procedures
NW_getBlockFunc NW_getBlock;
NW_setBlockFunc NW_setBlock;
NW_registerBlockFunc NW_registerBlock;

extern "C"
{

    // ### Export variables/procedures to plugins ###

    NWAPIEXPORT PL_BlockData getBlock(const PL_Vec3i* pos)
    {
        return convertBlockData((world->*NW_getBlock)(*pos));
    }
    NWAPIEXPORT void setBlock(const PL_Vec3i* pos, PL_BlockData block)
    {
        (world->*NW_setBlock)(*pos, convertBlockData(block));
    }
    NWAPIEXPORT void registerBlock(const PL_BlockType* block)
    {
        (blocks->*NW_registerBlock)(convertBlockType(*block));
    }

}

// Conversions between plugin structures and NEWorld structures
// This is used when structure definitions in NEWorld and in Plugin API are different
NW_BlockData convertBlockData(const PL_BlockData & src)
{
    return NW_BlockData(src.id, src.brightness, src.state);
}

PL_BlockData convertBlockData(const NW_BlockData & src)
{
    PL_BlockData res;
    res.id = src.getID();
    res.brightness = src.getBrightness();
    res.state = src.getState();
    return res;
}

NW_BlockType convertBlockType(const PL_BlockType & src)
{
    return NW_BlockType(src.blockname, src.solid, src.translucent, src.opaque, src.explodePower, src.hardness);
}
