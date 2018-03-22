// 
// GUI: nwapigui.hpp
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#pragma once

#include "ApiCore.h"
#include "Common/Filesystem.h"

extern "C" {

    // API related to GUI or renderer

    typedef size_t NWtextureid;
    typedef void (*NWblockrenderfunc)(void* cthis, NWblockdata data, int x, int y, int z);

    struct NWblocktexture {
        NWtextureid right, left, top, bottom, front, back;
    };

    NWAPI NWtextureid NWAPICALL nwRegisterTexture(const filesystem::path& pth);
    NWAPI void NWAPICALL nwSetBlockRenderFunc(size_t id, NWblockrenderfunc func);
    NWAPI void NWAPICALL nwUseDefaultBlockRenderFunc(size_t id, void* data);
}
