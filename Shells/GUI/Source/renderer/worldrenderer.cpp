// 
// GUI: worldrenderer.cpp
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

#include "worldrenderer.h"
#include "Game/Client/player.h"
#include "../renderdetector.hpp"

size_t WorldRenderer::render(const Int3& position) const {
    std::vector<std::pair<Int3, const ChunkRenderer*>> chunkPending;
    chunkPending.reserve(512);

    Renderer::startFrame();
    Int3 chunkPos = World::getChunkPos(position);
    for (auto& c : mChunkRenderers) {
        if (ChebyshevDistance(chunkPos, c.first) <= mRenderDist) {
            c.second.render(c.first);
            chunkPending.emplace_back(c.first, &c.second);
        }
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (auto& c : chunkPending) { c.second->renderTrans(c.first); }
    glDisable(GL_BLEND);
    Renderer::endFrame();
    return chunkPending.size();
}

void WorldRenderer::registerTask(Player& player) noexcept {
    TaskDispatch::addRegular(
            std::make_unique<RenderDetectorTask>(*this, mWorld.getWorldID(), player)
    );
}
