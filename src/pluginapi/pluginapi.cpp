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

extern "C"
{

    // Initialize PluginAPI with precedure pointers
    NWAPIEXPORT void init(NW_getBlockFunc NW_getBlock_, NW_setBlockFunc NW_setBlock_, NW_registerBlockFunc NW_registerBlock_)
    {
        NW_getBlock = NW_getBlock_;
        NW_setBlock = NW_setBlock_;
        NW_registerBlock = NW_registerBlock_;
    }

    // Export variables/procedures
    NWAPIEXPORT PL_BlockData getBlock(const PL_Vec3i* pos)
    { return NW_getBlock(*pos); }
    NWAPIEXPORT void setBlock(const PL_Vec3i* pos, PL_BlockData block)
    { NW_setBlock(*pos, block); }
    NWAPIEXPORT void registerBlock(const PL_BlockType* block)
    { NW_registerBlock(convertBlockType(*block)); }

}
