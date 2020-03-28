// 
// Core: world.cpp
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
#include <Game/SyncService/taskdispatcher.hpp>
#include <Game/SyncService/chunkservice.hpp>
#include <Game/Client/player.h>
#include <mutex>
#include "world.h"
#include "Common/JsonHelper.h"
#include "Common/OrderedList.h"

constexpr int MaxChunkLoadCount = 128, MaxChunkUnloadCount = 128;

size_t World::IDCount = 0;

std::vector<AABB> World::getHitboxes(const AABB& range) const {
    std::vector<AABB> res;
    Int3 curr;
    for (curr.X = int(floor(range.min.X)); curr.X < int(ceil(range.max.X)); curr.X++)
        for (curr.Y = int(floor(range.min.Y)); curr.Y < int(ceil(range.max.Y)); curr.Y++)
            for (curr.Z = int(floor(range.min.Z)); curr.Z < int(ceil(range.max.Z)); curr.Z++) {
                // TODO: BlockType::getAABB
                if (!isChunkLoaded(getChunkPos(curr)))
                    continue;
                if (getBlock(curr).getID() == 0)
                    continue;
                Double3 currd = curr;
                res.emplace_back(currd, currd + Double3(1.0, 1.0, 1.0));
            }
    return res;
}

static constexpr Int3 middleOffset() noexcept {
    return Int3(Chunk::Size() / 2 - 1, Chunk::Size() / 2 - 1, Chunk::Size() / 2 - 1);
}

/**
* \brief Find the nearest chunks in load range to load,
*        fartherest chunks out of load range to unload.
* \param world the world to load or unload chunks
* \param centerPos the center position
* \param loadRange chunk load range
* \param loadList (Output) chunk load list [position, distance]
* \param unloadList (Output) chunk unload list [pointer, distance]
*/
static void generateLoadUnloadList(
    const World& world, const Int3& centerPos, int loadRange,
    PodOrderedList<int, Int3, MaxChunkLoadCount>& loadList,
    PodOrderedList<int, Chunk*, MaxChunkUnloadCount, std::greater>& unloadList) {
    // centerPos to chunk coords
    Int3 centerCPos = World::getChunkPos(centerPos);

    for (auto&& chunk : world.getChunks()) {
        Int3 curPos = chunk.second->getPosition();
        // Out of load range, pending to unload
        if (ChebyshevDistance(centerCPos, curPos) > loadRange)
            unloadList.insert((curPos * Chunk::Size() + middleOffset() - centerPos).LengthSquared(), chunk.second.get());
    }

    for (int x = centerCPos.X - loadRange; x <= centerCPos.X + loadRange; x++)
        for (int y = centerCPos.Y - loadRange; y <= centerCPos.Y + loadRange; y++)
            for (int z = centerCPos.Z - loadRange; z <= centerCPos.Z + loadRange; z++)
                // In load range, pending to load
                if (!world.isChunkLoaded(Int3(x, y, z)))
                    loadList.insert((Int3(x, y, z) * Chunk::Size() + middleOffset() - centerPos).LengthSquared(),
                                    Int3(x, y, z));
}

class AddToWorldTask : public ReadWriteTask {
public:
    /**
     * \brief Add a constructed chunk into world.
     * \param worldID the target world's id
     * \param chunk the target chunk
     */
    AddToWorldTask(size_t worldID, std::unique_ptr<Chunk> chunk)
        : mWorldId(worldID), mChunk(std::move(chunk)) { }

    void task(ChunkService& cs) override {
        auto world = cs.getWorlds().getWorld(mWorldId);
        world->insertChunkAndUpdate(mChunk->getPosition(), std::move(mChunk));
    }

private:
    size_t mWorldId;
    std::unique_ptr<Chunk> mChunk;
};

class LoadFinishedTask : public ReadWriteTask {
public:
    /**
     * \brief Add a constructed chunk into world.
     * \param worldID the target world's id
     * \param chunk the target chunk
     */
    LoadFinishedTask(size_t worldID, Int3 chunkPos, std::vector<uint32_t> data)
        : mWorldId(worldID), mChunkPos(chunkPos), mData(std::move(data)) { }

    void task(ChunkService& cs) override {
        auto world = cs.getWorlds().getWorld(mWorldId);
        try {
            auto& chunk = world->getChunk(mChunkPos);
            chunk.replaceChunk(mData);
        }
        catch (std::out_of_range) {}
    }

private:
    size_t mWorldId;
    Int3 mChunkPos;
    std::vector<uint32_t> mData;
};

class UnloadChunkTask : public ReadWriteTask {
public:
    /**
    * \brief Given a chunk, it will try to unload it (decrease a ref)
    * \param world the target world
    * \param chunkPosition the position of the chunk
    */
    UnloadChunkTask(const World& world, Int3 chunkPosition)
        : mWorld(world), mChunkPosition(chunkPosition) { }

    void task(ChunkService& cs) override {
        //TODO: for multiplayer situation, it should decrease ref counter instead of deleting
        cs.getWorlds().getWorld(mWorld.getWorldID())->deleteChunk(mChunkPosition);
    }

private:
    const World& mWorld;
    Int3 mChunkPosition;
};

class BuildOrLoadChunkTask : public ReadOnlyTask {
public:
    /**
     * \brief Given a chunk, it will try to load it or build it
     * \param world the target world
     * \param chunkPosition the position of the chunk
     */
    BuildOrLoadChunkTask(const World& world, Int3 chunkPosition)
        : mWorld(world), mChunkPosition(chunkPosition) { }

    void task(const ChunkService& cs) override {
        if (mWorld.getChunks().isLoaded(mChunkPosition)) return;
        ChunkManager::data_t chunk;
        if (false) {
            // TODO: should try to load from local first

        }
        else {
            // Not found: build it!
            chunk = std::make_unique<Chunk>(mChunkPosition, mWorld);
        }
        // Add addToWorldTask
        TaskDispatch::addNext(std::make_unique<AddToWorldTask>(mWorld.getWorldID(), std::move(chunk))
        );
    }

private:
    const World& mWorld;
    Int3 mChunkPosition;
};

class LoadUnloadDetectorTask : public ReadOnlyTask {
public:
    LoadUnloadDetectorTask(World& world, const Player& player): mPlayer(player), mWorld(world) { }

    void task(const ChunkService& cs) override {
        // TODO: FIXME: Chunks might be repeatedly loaded or removed.

        PodOrderedList<int, Int3, MaxChunkLoadCount> loadList;
        PodOrderedList<int, Chunk*, MaxChunkUnloadCount, std::greater> unloadList;

        generateLoadUnloadList(mWorld, mPlayer.getPosition(),
                               getJsonValue<size_t>(getSettings()["server"]["load_distance"], 4),
                               loadList, unloadList);

        for (auto& loadPos : loadList) {
            if (cs.isAuthority()) {
                TaskDispatch::addNow(
                        std::make_unique<BuildOrLoadChunkTask>(mWorld, loadPos.second)
                );
            }
            else {
                //TODO: implement load chunk
            }
        }
        for (auto& unloadChunk : unloadList) {
            // add a unload task.
            TaskDispatch::addNext(
                    std::make_unique<UnloadChunkTask>(mWorld, unloadChunk.second->getPosition())
            );
        }
    }

    std::unique_ptr<ReadOnlyTask> clone() override { return std::make_unique<LoadUnloadDetectorTask>(*this); }

private:
    const Player& mPlayer;
    const World& mWorld;
};

void World::registerChunkTasks(Player& player) {
    // LoadUnloadDetectorTask
    TaskDispatch::addRegular(
            std::make_unique<LoadUnloadDetectorTask>(*this, player)
    );
}
