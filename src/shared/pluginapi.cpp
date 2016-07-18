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

BlockManager* PiBlocks;

int NWAPICALL nwRegisterBlock(const NWblocktype* block)
{
    PiBlocks->registerBlock(convertBlockType(*block));
    return 0;
}

BlockType convertBlockType(const NWblocktype& src)
{
    return BlockType(src.blockname, src.solid, src.translucent, src.opaque, src.explodePower, src.hardness);
}

/*

extern "C"
{

    // ### Export variables/procedures to plugins ###

    NWAPIEXPORT PluginAPI::PiBlockData getBlock(const PluginAPI::PiVec3i* pos)
    {
        return PluginAPI::convertBlockData((world->getBlock)(*pos));
    }
    NWAPIEXPORT void setBlock(const PluginAPI::PiVec3i* pos, PluginAPI::PiBlockData block)
    {
        (world->setBlock)(*pos, PluginAPI::convertBlockData(block));
    }
    NWAPIEXPORT void registerBlock(const PluginAPI::PiBlockType* block)
    {
        (blocks->registerBlock)(PluginAPI::convertBlockType(*block));
    }

}

// Conversions between plugin structures and NEWorld structures
// This is used when structure definitions in NEWorld and in Plugin API are different
BlockData PluginAPI::convertBlockData(const PiBlockData& src)
{
    return BlockData(src.id, src.brightness, src.state);
}

PluginAPI::PiBlockData PluginAPI::convertBlockData(const BlockData& src)
{
    PiBlockData res;
    res.id = src.getID();
    res.brightness = src.getBrightness();
    res.state = src.getState();
    return res;
}

BlockType PluginAPI::convertBlockType(const PiBlockType& src)
{
    return BlockType(src.blockname, src.solid, src.translucent, src.opaque, src.explodePower, src.hardness);
}

*/
