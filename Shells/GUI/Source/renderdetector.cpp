// 
// GUI: renderdetector.cpp
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

#include "renderdetector.hpp"
#include "Game/Client/player.h"

static bool neighbourChunkLoadCheck(const World& world, const Vec3i& pos) {
    constexpr std::array<Vec3i, 6> delta
    {
        Vec3i(1, 0, 0), Vec3i(-1, 0, 0), Vec3i(0, 1, 0),
        Vec3i(0, -1, 0), Vec3i(0, 0, 1), Vec3i(0, 0, -1)
    };
    for (auto&& p : delta)
        if (!world.isChunkLoaded(pos + p))
            return false;
    return true;
}

class VBOGenerateTask : public RenderTask {
public:
    VBOGenerateTask(const World& world, const Vec3i& position, ChunkRenderData crd,
                    std::unordered_map<Vec3i, ChunkRenderer>& chunkRenderers)
        : mWorld(world), mPosition(position), mChunkRenderData(std::move(crd)),
          mChunkRenderers(chunkRenderers) { }

    void task(const ChunkService& cs) override {
        try { mWorld.getChunks()[mPosition].setUpdated(false); }
        catch (std::out_of_range&) {
            return; // chunk is unloaded
        }
        auto it = mChunkRenderers.find(mPosition);
        if (it != mChunkRenderers.end()) { it->second = ChunkRenderer(mChunkRenderData); }
        else { mChunkRenderers.insert(std::pair<Vec3i, ChunkRenderer>(mPosition, ChunkRenderer(mChunkRenderData))); }
    }

private:
    const World& mWorld;
    Vec3i mPosition;
    ChunkRenderData mChunkRenderData;
    std::unordered_map<Vec3i, ChunkRenderer>& mChunkRenderers;
};

class ChunkRenderDataGenerateTask : public ReadOnlyTask {
public:
    ChunkRenderDataGenerateTask(WorldRenderer& worldRenderer, size_t currentWorldID, Vec3i chunkPos) :
            mWorldRenderer(worldRenderer), mCurrentWorldId(currentWorldID), mChunkPos(chunkPos) { }

    void task(const ChunkService& cs) override;

    std::unique_ptr<ReadOnlyTask> clone() override { return std::make_unique<ChunkRenderDataGenerateTask>(*this); }

private:
    WorldRenderer& mWorldRenderer;
    size_t mCurrentWorldId;
    Vec3i mChunkPos;
};

void RenderDetectorTask::task(const ChunkService& cs) {
    int counter = 0;
    // TODO: improve performance by adding multiple instances of this and set a step when itering the chunks.
    // Render build list
    //PODOrderedList<int, chunk*, MaxChunkRenderCount> chunkRenderList;
    Vec3i chunkpos = World::getChunkPos(mPlayer.getPosition());
    auto world = cs.getWorlds().getWorld(mCurrentWorldId);
    for (const auto& c : world->getChunks()) {
        auto& chunk = c.second;
        auto chunkPosition = chunk->getPosition();
        // In render range, pending to render
        if (chunk->isUpdated() &&
            chunkpos.chebyshevDistance(chunkPosition) <= mWorldRenderer.mRenderDist) {
            if (neighbourChunkLoadCheck(*world, chunkPosition)) {
                TaskDispatch::addNow(
                        std::make_unique<ChunkRenderDataGenerateTask>(mWorldRenderer, mCurrentWorldId,
                                chunk->getPosition())
                );
                //wwwwif (counter++ == mMaxChunkLoadPerTick) break;
            }
        }
        else {
            // TODO: Unload unneeded VBO.
            //       We can't do it here since it's not thread safe
            /*
            auto iter = mChunkRenderers.find(chunkPosition);
            if (iter != mChunkRenderers.end())
            mChunkRenderers.erase(iter);
            */
        }
    }
}

void ChunkRenderDataGenerateTask::task(const ChunkService& cs)
{
    // TODO: maybe build a VA pool can speed this up.
    auto world = cs.getWorlds().getWorld(mCurrentWorldId);
    auto& chunk = world->getChunks()[mChunkPos];
    if (!neighbourChunkLoadCheck(*world, mChunkPos)) return;
    ChunkRenderData crd;
    crd.generate(&chunk);
    TaskDispatch::addNext(
            std::make_unique<VBOGenerateTask>(*world, mChunkPos, std::move(crd), mWorldRenderer.mChunkRenderers)
    );
}
