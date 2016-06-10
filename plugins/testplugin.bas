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

#include "../api/freebasic/nwapi.bi"

' Plugin constants
const PluginName as string = "infinideas.testplugin"

' NEWorld constants
const ChunkSize as integer = 32
const AirID as integer = 0
const BrightnessSolidBlock as ineteger = 0
const BrightnessMin as integer = 1
const BrightnessMax as integer = 15

' Block IDs
namespace Blocks
    
    dim as integer Air = AirID, Rock, Soil, Grass, Stone, Wood, Leaf, Water
    
    sub getID()
        ' TODO: [NEWorld] NWAPIENTRY int getBlockID(const char*)
        ' TODO: Get block ID
    end sub
    
end namespace

' Convert const string to zstring ptr
function c_str(byref s as const string) as zstring ptr
    dim res as zstring ptr = new byte[len(s) + 1]
    *res = s
    return res
end function

' Add block
sub addblock(byref blockname as const string, _
    byval solid as byte, byval translucent as byte, byval opaque as byte, _
    byval explodePower as int32, byval hardness as int32)
    dim block as BlockType
    with block
        block.blockname = c_str(blockname)
        block.solid = solid
        block.translucent = translucent
        block.opaque = opaque
        block.explodePower = explodePower
        block.hardness = hardness
    end with
    registerBlock(@block)
end sub

' Chunk generator
function buildChunk(byref cpos as const Vec3i) as BlockData ptr
    dim skyBrightness as integer = 0 ' TODO: [NEWorld] NWAPIENTRY int getSkyBrightness()
    dim res as BlockData ptr = new BlockData[ChunkSize*ChunkSize*ChunkSize]
    dim i as integer
    if cpos.y<=0 then
        ' Ground
        for i=0 to ChunkSize*ChunkSize*ChunkSize - 1
            res[i].id = Blocks::Rock
            res[i].brightness = BrightnessSolidBlock
        next
    else
        ' Air
        for i=0 to ChunkSize*ChunkSize*ChunkSize - 1
            res[i].id = Blocks::Air
            res[i].brightness = skyBrightness
        next
    end if
    return res
end function

' Main functions
function init() as PluginData ptr
    addBlock("rock", 1, 0, 1, 0, 10)
    addBlock("soil", 1, 0, 1, 0, 5)
    addBlock("grass", 1, 0, 1, 0, 5)
    addBlock("stone", 1, 0, 1, 0, 10)
    addBlock("wood", 1, 0, 1, 0, 8)
    addBlock("leaf", 1, 0, 0, 0, 3)
    addBlock("water", 0, 1, 0, 0, 0)
    dim testPlugin as PluginData ptr = new PluginData
    testPlugin->pluginName = c_str(PluginName)
    testPlugin->buildChunk = @buildChunk
    return testPlugin
end function

sub postInit() ' TODO: [NEWorld] Call postInit() after all plugins were loaded
    Blocks::getID()
end sub
