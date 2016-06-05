/'
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
'/

#ifndef PLUGINAPI_BI_
#define PLUGINAPI_BI_

type int32 as long
type uint32 as unsigned long

type Vec3i
    dim as int32 x, y, z
end type

type BlockType
    dim blockname as zstring ptr = 0
    dim solid as byte
    dim translucent as byte
    dim opaque as byte
    dim explodePower as int32
    dim hardness as int32
end type

type BlockData
    dim id : 12 as uint32
    dim brightness : 4 as uint32
    dim state : 16 as uint32
end type

type buildChunkFunc as function(byref as const Vec3i) as BlockData ptr

type PluginData
    dim pluginName as zstring ptr = 0
    dim blocksCount as int32
    dim blocks as BlockType ptr = 0
    dim buildChunk as buildChunkFunc = 0
end type

extern "C"
    declare function getBlock(byval as const Vec3i ptr) as BlockData
    declare sub setBlock(byval as const Vec3i ptr, byval as BlockData)
    declare sub registerBlock(byval as const BlockType ptr)
end extern

#endif
