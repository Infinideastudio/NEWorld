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

namespace NEWorld
{
    class Vec3i
    {
    public:
        int x, y, z;
    };

    class BlockType
    {
    public:
        const char* blockname = nullptr;
        bool solid;
        bool translucent;
        bool opaque;
        int explodePower;
        int hardness;
    };

    class BlockData
    {
    public:
        unsigned int id : 12;
        unsigned int brightness : 4;
        unsigned int state : 16;
    };

    class PluginData
    {
    public:
        char* pluginName;
        int blocksCount;
        BlockType* blocks = nullptr;
        BlockData* (*buildChunk)(const Vec3i&) = nullptr;
    };
}

#endif
