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

#include "../src/api/freebasic/pluginapi.bi"

using NEWorld

' Plugin attributes
dim shared pluginName as zstring*22 = "infinideas.testplugin"
dim shared testPlugin as PluginData
dim shared blocks as BlockType ptr

' Block attributes storage
dim shared name_rock as zstring*5 = "rock"
dim shared name_soil as zstring*5 = "soil"
dim shared name_grass as zstring*6 = "grass"

' NEWorld constants
const ChunkSize as integer = 32

' Add block
sub registerblock(byval index as integer, byval blockname as byte ptr, _
    byval solid as byte, byval translucent as byte, byval opaque as byte, _
    byval explodePower as int32_t, byval hardness as int32_t)
    blocks[index].blockname = blockname
    blocks[index].solid = solid
    blocks[index].translucent = translucent
    blocks[index].opaque = opaque
    blocks[index].explodePower = explodePower
    blocks[index].hardness = hardness
end sub

' Chunk generator
function buildChunk(byref cpos as const Vec3i) as BlockData ptr
    dim res as BlockData ptr = callocate(ChunkSize*ChunkSize*ChunkSize, sizeof(BlockData))
    if cpos.y>=0 then
        dim i as integer
        for i=0 to ChunkSize*ChunkSize*ChunkSize - 1
            res[i].id = 0' NOT FINISHED
            res[i].brightness = 0' NOT FINISHED
        next
    end if
    return res
end function

' Main function
function init() as PluginData ptr
    blocks = callocate(3, sizeof(BlockType))
    registerblock(0, @name_rock, 1, 0, 1, 0, 10)
    registerBlock(1, @name_soil, 1, 0, 1, 0, 5)
    registerBlock(2, @name_grass, 1, 0, 1, 0, 5)
    testPlugin.pluginName = @pluginName
    testPlugin.blocksCount = 3
    testPlugin.blocks = blocks
    testPlugin.buildChunk = @buildchunk
    return @testPlugin
end function
