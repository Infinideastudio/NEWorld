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
#include <memory>
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <algorithm>
#include "Game/World/Chunk/Chunk.h"
#include "Common/Physics/AABB.h"
#include "WorldStorage.h"
#include "Blocks.h"

class Player;
class ChunkService;
class ModuleManager;

class NWCOREAPI World final : public NonCopyable {
public:
    World(std::string name, const Game::World::Blocks& blocks)
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
    [[nodiscard]] const std::string& getWorldName() const noexcept { return mName; }
    [[nodiscard]] size_t getWorldID() const noexcept { return mID; }
    [[nodiscard]] int getDaylightBrightness() const noexcept { return mDaylightBrightness; }

    ////////////////////////////////////////
    // chunk Management
    ////////////////////////////////////////
    using ChunkIterator = ChunkManager::iterator;
    using ChunkReference = ChunkManager::reference;
    // Raw Access
    ChunkManager& getChunks() noexcept { return mChunks; }
    [[nodiscard]] const ChunkManager& getChunks() const noexcept { return mChunks; }
    // Alias declearations for Chunk management
    [[nodiscard]] size_t getChunkCount() const { return mChunks.size(); }
    ChunkReference getChunk(const Int3& ChunkPos) { return mChunks[ChunkPos]; }
    [[nodiscard]] bool isChunkLoaded(const Int3& ChunkPos) const noexcept { return mChunks.isLoaded(ChunkPos); }
    void deleteChunk(const Int3& ChunkPos) {
        // TODO: check if the chunk needs to be saved.
        if (!isChunkLoaded(ChunkPos)) return;
        mWorldStorage.saveChunk(ChunkPos, getChunk(ChunkPos).getChunkForExport());
        mChunks.erase(ChunkPos);
    }
    static int getChunkAxisPos(int pos) { return ChunkManager::getAxisPos(pos); }
    static Int3 getChunkPos(const Int3& pos) { return ChunkManager::getPos(pos); }
    static int getBlockAxisPos(int pos) { return ChunkManager::getBlockAxisPos(pos); }
    static Int3 getBlockPos(const Int3& pos) { return ChunkManager::getBlockPos(pos); }
    [[nodiscard]] Game::World::BlockData getBlock(const Int3& pos) const { return mChunks.getBlock(pos); }
    void setBlock(const Int3& pos, Game::World::BlockData block) { mChunks.setBlock(pos, block); }
    auto insertChunk(const Int3& pos, ChunkManager::data_t&& ptr) { return mChunks.insert(pos, std::move(ptr)); }

    auto insertChunkAndUpdate(const Int3& pos, ChunkManager::data_t&& ptr) {
        const auto chunkPosition = ptr->getPosition();
        const auto ret = insertChunk(pos, std::move(ptr));
        constexpr std::array<Int3, 6> delta
        {
            Int3(1, 0, 0), Int3(-1, 0, 0),
            Int3(0, 1, 0), Int3(0, -1, 0),
            Int3(0, 0, 1), Int3(0, 0, -1)
        };
        for (auto&& p : delta)
            doIfChunkLoaded(chunkPosition + p, [](Chunk& chk) { chk.setUpdated(true); });
        return ret;
    }

    auto resetChunk(const Int3& pos, Chunk* ptr) { return mChunks.reset(pos, ptr); }

    template <typename... ArgType, typename Func>
    void doIfChunkLoaded(const Int3& ChunkPos, Func func, ArgType&&... args) {
        mChunks.doIfLoaded(ChunkPos, func, std::forward<ArgType>(args)...);
    };

    // Add Chunk
    Chunk* addChunk(const Int3& chunkPos) {
        return insertChunk(chunkPos, std::make_unique<Chunk>(chunkPos, *this))->second.get();
    }

    [[nodiscard]] std::vector<AABB> getHitboxes(const AABB& range) const;

    // Tasks
    void registerChunkTasks(Player& player);
protected:
    // World name
    std::string mName;
    // World ID
    size_t mID;
    static size_t IDCount;
    // Loaded blocks
    const Game::World::Blocks& mBlocks;
    // All Chunks (chunk array)
    ChunkManager mChunks;
    int mDaylightBrightness;
    WorldStorage mWorldStorage;
};


class WorldManager: public NonCopyable {
public:
    WorldManager(Game::World::Blocks& blocks) :mBlocks(blocks) { }

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

    [[nodiscard]] size_t size() const noexcept { return mWorlds.size(); }

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

    [[nodiscard]] const World* getWorld(const std::string& name) const {
        for (auto&& world : *this)
            if (world->getWorldName() == name) return world.get();
        return nullptr;
    }

    [[nodiscard]] const World* getWorld(size_t id) const {
        for (auto&& world : *this)
            if (world->getWorldID() == id) return world.get();
        return nullptr;
    }

private:
    std::vector<std::unique_ptr<World>> mWorlds;
    Game::World::Blocks& mBlocks;
};
