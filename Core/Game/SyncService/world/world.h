// 
// Core: world.h
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

#include <array>
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <algorithm>
#include "nwchunk.h"
#include "Common/Physics/AABB.h"
#include "Game/SyncService/WorldStorage.h"

class Player;
class ChunkService;
class ModuleManager;

class NWCOREAPI World final : public NonCopyable {
public:
    World(std::string name, const Blocks& blocks)
        : mName(std::move(name)), mID(0), mBlocks(blocks), mChunks(1024), mDaylightBrightness(15),
          mWorldStorage(mName) { }

    World(World&& rhs) noexcept
        : mName(std::move(rhs.mName)), mID(rhs.mID), mBlocks(rhs.mBlocks),
          mChunks(std::move(rhs.mChunks)), mDaylightBrightness(rhs.mDaylightBrightness),
          mWorldStorage(mName) { }

    ~World() = default;

    ////////////////////////////////////////
    // World Properties
    ////////////////////////////////////////
    const std::string& getWorldName() const noexcept { return mName; }
    size_t getWorldID() const noexcept { return mID; }
    int getDaylightBrightness() const noexcept { return mDaylightBrightness; }

    ////////////////////////////////////////
    // Chunk Management
    ////////////////////////////////////////
    using ChunkIterator = ChunkManager::iterator;
    using ChunkReference = ChunkManager::reference;
    // Raw Access
    ChunkManager& getChunks() noexcept { return mChunks; }
    const ChunkManager& getChunks() const noexcept { return mChunks; }
    // Alias declearations for Chunk management
    size_t getChunkCount() const { return mChunks.size(); }
    ChunkReference getChunk(const Vec3i& ChunkPos) { return mChunks[ChunkPos]; }
    bool isChunkLoaded(const Vec3i& ChunkPos) const noexcept { return mChunks.isLoaded(ChunkPos); }
    void deleteChunk(const Vec3i& ChunkPos) {
        // TODO: check if the chunk needs to be saved.
        if (!isChunkLoaded(ChunkPos)) return;
        mWorldStorage.saveChunk(ChunkPos, getChunk(ChunkPos).getChunkForExport());
        mChunks.erase(ChunkPos);
    }
    static int getChunkAxisPos(int pos) { return ChunkManager::getAxisPos(pos); }
    static Vec3i getChunkPos(const Vec3i& pos) { return ChunkManager::getPos(pos); }
    static int getBlockAxisPos(int pos) { return ChunkManager::getBlockAxisPos(pos); }
    static Vec3i getBlockPos(const Vec3i& pos) { return ChunkManager::getBlockPos(pos); }
    BlockData getBlock(const Vec3i& pos) const { return mChunks.getBlock(pos); }
    void setBlock(const Vec3i& pos, BlockData block) { mChunks.setBlock(pos, block); }
    auto insertChunk(const Vec3i& pos, ChunkManager::data_t&& ptr) { return mChunks.insert(pos, std::move(ptr)); }

    auto insertChunkAndUpdate(const Vec3i& pos, ChunkManager::data_t&& ptr) {
        const auto chunkPosition = ptr->getPosition();
        const auto ret = insertChunk(pos, std::move(ptr));
        constexpr std::array<Vec3i, 6> delta
        {
            Vec3i(1, 0, 0), Vec3i(-1, 0, 0),
            Vec3i(0, 1, 0), Vec3i(0, -1, 0),
            Vec3i(0, 0, 1), Vec3i(0, 0, -1)
        };
        for (auto&& p : delta)
            doIfChunkLoaded(chunkPosition + p, [](Chunk& chk) { chk.setUpdated(true); });
        return ret;
    }

    auto resetChunk(const Vec3i& pos, Chunk* ptr) { return mChunks.reset(pos, ptr); }

    template <typename... ArgType, typename Func>
    void doIfChunkLoaded(const Vec3i& ChunkPos, Func func, ArgType&&... args) {
        mChunks.doIfLoaded(ChunkPos, func, std::forward<ArgType>(args)...);
    };

    // Add Chunk
    Chunk* addChunk(const Vec3i& chunkPos,
                    ChunkOnReleaseBehavior::Behavior behv = ChunkOnReleaseBehavior::Behavior::Release) {
        return insertChunk(
                   chunkPos, ChunkManager::data_t(new Chunk(chunkPos, *this), ChunkOnReleaseBehavior(behv)))
               ->second.get();
    }


    ////////////////////////////////////////
    // BlockType Management
    ////////////////////////////////////////
    const Blocks& getBlockTypes() const { return mBlocks; }
    const BlockType& getType(int id) const { return mBlocks[id]; }

    std::vector<AABB> getHitboxes(const AABB& range) const;

    // Tasks
    void registerChunkTasks(Player& player);
private:
    /**
     * \brief Find chunks that needs to be loaded or unloaded
     * \param playerPosition the position of the player, which will be used
     *                       as the center position.
     * \note Read-only and does not involve OpenGL operation.
     *       *this will be used as the target world.
     * TODO: change to adapt multiplayer mode
     */
    void loadUnloadDetector(const Vec3i& playerPosition) const;

protected:
    // World name
    std::string mName;
    // World ID
    size_t mID;
    static size_t IDCount;
    // Loaded blocks
    const Blocks& mBlocks;
    // All Chunks (Chunk array)
    ChunkManager mChunks;
    int mDaylightBrightness;
    WorldStorage mWorldStorage;
};


class WorldManager: public NonCopyable {
public:
    WorldManager(Blocks& blocks) : mBlocks(blocks) { }

    ~WorldManager() { mWorlds.clear(); }

    void clear() { mWorlds.clear(); }

    World* addWorld(const std::string& name) {
        mWorlds.emplace_back(new World(name, mBlocks));
        return mWorlds[mWorlds.size() - 1].get();
    }

    [[nodiscard]] auto begin() { return mWorlds.begin(); }
    [[nodiscard]] auto end() { return mWorlds.end(); }
    [[nodiscard]] auto begin() const { return mWorlds.cbegin(); }
    [[nodiscard]] auto end() const { return mWorlds.cend(); }

    size_t size() const noexcept { return mWorlds.size(); }

    World* getWorld(const std::string& name) {
        for (auto&& world : *this)
            if (world->getWorldName() == name) return world.get();
        return nullptr;
    }

    World* getWorld(size_t id) {
        for (auto&& world : *this)
            if (world->getWorldID() == id) return world.get();
        return nullptr;
    }

    const World* getWorld(const std::string& name) const {
        for (auto&& world : *this)
            if (world->getWorldName() == name) return world.get();
        return nullptr;
    }

    const World* getWorld(size_t id) const {
        for (auto&& world : *this)
            if (world->getWorldID() == id) return world.get();
        return nullptr;
    }

private:
    std::vector<std::unique_ptr<World>> mWorlds;
    Blocks& mBlocks;
};
