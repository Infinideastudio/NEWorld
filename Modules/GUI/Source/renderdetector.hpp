// 
// GUI: renderdetector.hpp
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
#include "Game/SyncService/taskdispatcher.hpp"
#include "renderer/worldrenderer.h"

class RenderDetectorTask : public ReadOnlyTask {
public:
    RenderDetectorTask(WorldRenderer& worldRenderer, size_t currentWorldID, const Player& player) :
        mWorldRenderer(worldRenderer), mCurrentWorldId(currentWorldID), mPlayer(player) { }

    void task(const ChunkService& cs) override;

    std::unique_ptr<ReadOnlyTask> clone() override { return std::make_unique<RenderDetectorTask>(*this); }

private:
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

    WorldRenderer& mWorldRenderer;
    size_t mCurrentWorldId;
    const Player& mPlayer;
};
