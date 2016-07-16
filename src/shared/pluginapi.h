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

#ifndef PLUGINAPI_H_
#define PLUGINAPI_H_

#include "common.h"
#include "logger.h"
#include "vec3.h"
#include "blockdata.h"
#include "blocktype.h"
#include "blockmanager.h"
#include "world.h"

extern BlockManager* PiBlocks;

struct NWblocktype
{
    char* blockname = nullptr;
    bool solid;
    bool translucent;
    bool opaque;
    int32_t explodePower;
    int32_t hardness;
};

BlockType convertBlockType(const NWblocktype& src);

extern "C"
{
    NWAPIEXPORT int NWAPICALL nwRegisterBlock(const NWblocktype*);
}

/*

// Plugin interface
class PluginAPI
{
public:

    // Structures for plugin interface
    // Aliases cannot be used when structure definitions in NEWorld and in Plugin API are different
    using PiVec3i = Vec3i;

    struct PiBlockData
    {
        uint32_t id : 12;
        uint32_t brightness : 4;
        uint32_t state : 16;
    };

    struct PiBlockType
    {
        char* blockname = nullptr;
        bool solid;
        bool translucent;
        bool opaque;
        int32_t explodePower;
        int32_t hardness;
    };

    // Conversions between plugin structures and NEWorld structures
    // This is used when structure definitions in NEWorld and in Plugin API are different
    static BlockData convertBlockData(const PiBlockData& src);
    static PiBlockData convertBlockData(const BlockData& src);
    static BlockType convertBlockType(const PiBlockType& src);

    void setCurrWorld(World& world)
    {
        m_world = &world;
    }

    World& getCurrWorld()
    {
        return *m_world;
    }

private:
    World* m_world; // Current world
};

*/

#endif
