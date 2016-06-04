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

' NOT FINISHED

#include "../api/freebasic/pluginapi.bi"

' Plugin constants
const PluginName as string = "infinideas.testplugin"

' Plugin data
dim shared blocks as BlockType ptr

' NEWorld constants
const ChunkSize as integer = 32

' Convert const string to zstring ptr
function c_str(byref s as const string) as zstring ptr
    dim res as zstring ptr = new byte[len(s) + 1]
    *res = s
    return res
end function

' Chunk generator
function buildChunk(byref cpos as const Vec3i) as BlockData ptr
    dim res as BlockData ptr = new BlockData[ChunkSize*ChunkSize*ChunkSize]
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
    blocks = new BlockType[3]
    registerBlock(0, "rock", 1, 0, 1, 0, 10)
    registerBlock(1, "soil", 1, 0, 1, 0, 5)
    registerBlock(2, "grass", 1, 0, 1, 0, 5)
    dim testPlugin as PluginData ptr = new PluginData
    testPlugin->pluginName = c_str(PluginName)
    testPlugin->blocksCount = 3
    testPlugin->blocks = blocks
    testPlugin->buildChunk = @buildChunk
    return testPlugin
end function
