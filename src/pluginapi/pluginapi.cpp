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

#if defined _WIN32 || defined __CYGWIN__
    #ifdef _MSC_VER
        #define NWAPIEXPORT __declspec(dllexport)
    #else
        #define NWAPIEXPORT __attribute__ ((dllexport))
    #endif
#else
    #define NWAPIEXPORT __attribute__ ((visibility ("default")))
#endif

#include "../shared/vec3.h"
#include "../shared/blockdata.h"
#include "../shared/blocktype.h"

extern "C"
{
    
    // Pointer to procedure types
    typedef BlockData(*NWFgetBlock)(const Vec3i&);
    typedef void (*NWFsetBlock)(const Vec3i&, BlockData);

    // Pointers to NEWorld procedures
    NWFgetBlock NWgetBlock;
    NWFsetBlock NWsetBlock;

    // Initialize PluginAPI with precedure pointers
    NWAPIEXPORT void initPluginAPI(NWFgetBlock NWgetBlock_, NWFsetBlock NWsetBlock_)
    {
        NWgetBlock = NWgetBlock_;
        NWsetBlock = NWsetBlock_;
    }

    // Export procedures
    NWAPIEXPORT BlockData getBlock(const Vec3i* pos)
    { return NWgetBlock(*pos); }
    NWAPIEXPORT void setBlock(const Vec3i* pos, BlockData block)
    { NWsetBlock(*pos, block); }

}
