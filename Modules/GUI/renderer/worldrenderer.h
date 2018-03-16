// 
// GUI: worldrenderer.h
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
#include <unordered_map>
#include "chunkrenderer.h"
#include "Game/SyncService/chunkservice.hpp"

const int MaxChunkRenderCount = 4;

/**
 * \brief Manage the VBO of a world. It includes ChunkRenderer.
 */
class WorldRenderer {
public:
    WorldRenderer(const World& world, int renderDistance)
        : mWorld(world), mRenderDist(renderDistance) { }

    // Render all chunks
    size_t render(const Vec3i& position) const;

    void registerTask(ChunkService& chunkService, Player& player) noexcept;

    friend class RenderDetectorTask;

private:
    const World& mWorld;
    // Ranges
    int mRenderDist = 0;
    // Chunk Renderers
    std::unordered_map<Vec3i, ChunkRenderer> mChunkRenderers;
};
