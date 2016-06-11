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

#include "../shared/common.h"
#include "../shared/vec3.h"
#include "../shared/blockdata.h"
#include "../shared/blocktype.h"
#include "../shared/blockmanager.h"
#include "../shared/world.h"

// NWAPIEXPORT
#ifdef NEWORLD_TARGET_WINDOWS
#ifdef NEWORLD_COMPILER_MSVC
#define NWAPIEXPORT __declspec(dllexport)
#else
#define NWAPIEXPORT __attribute__((dllexport))
#endif
#else
#define NWAPIEXPORT __attribute__((visibility("default")))
#endif

// Prefix NW_ means it is an interface to NEWorld main program
// Prefix PL_ means it is an interface to plugins

// NEWorld structures
using NW_Vec3i = Vec3i;
using NW_BlockData = BlockData;
using NW_BlockType = BlockType;

// Structures for plugin interface
// Aliases cannot be used when structure definitions in NEWorld and in Plugin API are different
using PL_Vec3i = NW_Vec3i;

struct PL_BlockData
{
    uint32_t id : 12;
    uint32_t brightness : 4;
    uint32_t state : 16;
};

struct PL_BlockType
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
NW_BlockData convertBlockData(const PL_BlockData& src);

PL_BlockData convertBlockData(const NW_BlockData& src);

NW_BlockType convertBlockType(const PL_BlockType& src);

// Pointer to procedure types
typedef NW_BlockData(World::*NW_getBlockFunc)(const NW_Vec3i&) const;
typedef void(World::*NW_setBlockFunc)(const NW_Vec3i&, NW_BlockData);
typedef void(BlockManager::*NW_registerBlockFunc)(const NW_BlockType& block);

// Pointers to NEWorld procedures
extern NW_getBlockFunc NW_getBlock;
extern NW_setBlockFunc NW_setBlock;
extern NW_registerBlockFunc NW_registerBlock;

#endif
