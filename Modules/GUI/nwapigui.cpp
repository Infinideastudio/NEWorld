// 
// GUI: nwapigui.cpp
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

#include <memory>
#include "renderer/blockrenderer.h"
#include <api/nwapigui.hpp>
#include <Common/EventBus.h>
#include "Common/Logger.h"

extern "C" {
    NWAPI void NWAPICALL nwSetBlockRenderFunc(size_t, NWblockrenderfunc) { }

    NWAPI void NWAPICALL nwUseDefaultBlockRenderFunc(size_t id, void* data) {
        NWblocktexture* ptr = reinterpret_cast<NWblocktexture*>(data);
        size_t array[] = {ptr->right, ptr->left, ptr->top, ptr->bottom, ptr->front, ptr->back};
        BlockRendererManager::setBlockRenderer(id, std::make_shared<DefaultBlockRenderer>(array));
    }

    NWAPI NWtextureid NWAPICALL nwRegisterTexture(const char* filename) {
        return BlockTextureBuilder::addTexture(filename);
    }
}

void registerGUIAPI() {
    REGISTER_AUTO(nwSetBlockRenderFunc);
    REGISTER_AUTO(nwUseDefaultBlockRenderFunc);
    REGISTER_AUTO(nwRegisterTexture);
    debugstream << "GUI API functions registered!";
}
