// 
// Core: nwchunk.h
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

#include <mutex>
#include <atomic>
#include <chrono>
#include <memory>
#include <vector>
#include <utility>
#include <array>
#include <unordered_map>
#include "Common/Debug.h"
#include <Math/Vector3.h>
#include "Common/Utility.h"
#include "Game/World/Blocks.h"

class Chunk;

struct ChunkGenerateArgs {
    const Int3* pos;
    Chunk* const chunk;
    const int skyLight;
};

using ChunkGenerator = std::add_pointer_t<void(const ChunkGenerateArgs*)>;

class NWCOREAPI Chunk final : public NonCopyable {
public:
    // chunk size
    static bool ChunkGeneratorLoaded;
    static ChunkGenerator ChunkGen;
    static constexpr int BlocksSize = 32 * 32 * 32;
    static constexpr int SizeLog2() { return 5; }
    static constexpr int Size() { return 32; };
    using Blocks = std::array<Game::World::BlockData, BlocksSize>;
    using ChunkDataStorageType = std::vector<uint32_t>;

    enum class LoadBehavior { Build, Loading };

    explicit Chunk(const Int3& position, const class World& world, LoadBehavior behavior = LoadBehavior::Build);
    explicit Chunk(const Int3& position, const class World& world, const ChunkDataStorageType& data);
    ~Chunk() = default;

    // Get chunk position
    const Int3& getPosition() const noexcept { return mPosition; }

    // Get chunk updated flag
    bool isUpdated() const noexcept { return mUpdated; }

    bool isModified() const noexcept { return mModified; }

    // Get chunk loading status
    bool isLoading() const noexcept { return mLoading; }
    void replaceChunk(const ChunkDataStorageType& data) noexcept;

    // Set chunk updated flag
    void setUpdated(bool updated) const noexcept { mUpdated = updated; }

    // Get block data in this chunk
    Game::World::BlockData getBlock(const Int3& pos) const {
        Assert(pos.X >= 0 && pos.X < Size() && pos.Y >= 0 && pos.Y < Size() && pos.Z >= 0 && pos.Z < Size());
        return !isMonotonic() ? (*getBlocks())[pos.X * Size() * Size() + pos.Y * Size() + pos.Z] : mMonotonicBlock;
    }

    // Get block pointer. Note that they might return nullptr in case of monotonic chunk.
    Blocks* getBlocks() noexcept { return mBlocks.get(); }
    const Blocks* getBlocks() const noexcept { return mBlocks.get(); }
    ChunkDataStorageType getChunkForExport() const noexcept;

    // Set block data in this chunk
    void setBlock(const Int3& pos, Game::World::BlockData block) {
        Assert(pos.X >= 0 && pos.X < Size() && pos.Y >= 0 && pos.Y < Size() && pos.Z >= 0 && pos.Z < Size());
        if (isMonotonic() && block != mMonotonicBlock) allocateBlocks();
        (*getBlocks())[pos.X * Size() * Size() + pos.Y * Size() + pos.Z] = block;
        mUpdated = true;
    }

    // Build chunk
    void build(int daylightBrightness);

    const World* getWorld() const noexcept { return mWorld; }
    bool isMonotonic() const noexcept { return mBlocks == nullptr; }
    Game::World::BlockData getMonotonicBlock() const noexcept { return mMonotonicBlock; }
    void allocateBlocks(bool fill = true) { /*Assert(isMonotonic());*/
        if (!isMonotonic()) return;
        mBlocks = std::make_unique<Blocks>();
        if (fill) {
            std::fill(mBlocks->begin(), mBlocks->end(), mMonotonicBlock);
        }
    }
    void setMonotonic(Game::World::BlockData block) noexcept { Assert(isMonotonic()); mMonotonicBlock = block; }

private:
    Int3 mPosition;
    
    std::unique_ptr<Blocks> mBlocks;
    Game::World::BlockData mMonotonicBlock;

    // TODO: somehow avoid it! not safe.
    mutable bool mUpdated = false;
    bool mModified = false;
    bool mLoading = false;
    const class World* mWorld;
};


struct ChunkHasher {
    constexpr size_t operator()(const Int3& t) const noexcept {
        return static_cast<size_t>(t.X * 23947293731 + t.Z * 3296467037 + t.Y * 1234577);
    }
};

class ChunkManager : public NonCopyable {
public:
    using data_t = std::unique_ptr<Chunk>;
    using array_t = std::unordered_map<Int3, data_t, ChunkHasher>;
    using iterator = array_t::iterator;
    using const_iterator = array_t::const_iterator;
    using reference = Chunk&;
    using const_reference = const Chunk&;
    ChunkManager() = default;
    ChunkManager(size_t size) { mChunks.reserve(size); }
    ChunkManager(ChunkManager&& rhs) noexcept : mChunks(std::move(rhs.mChunks)) {}
    ~ChunkManager() = default;
    // Access and modifiers
    [[nodiscard]] size_t size() const noexcept { return mChunks.size(); }
    iterator begin() noexcept { return mChunks.begin(); }
    iterator end() noexcept { return mChunks.end(); }
    [[nodiscard]] const_iterator begin() const noexcept { return mChunks.cbegin(); }
    [[nodiscard]] const_iterator end() const noexcept { return mChunks.cend(); }

    reference at(const Int3& chunkPos) { return *(mChunks.at(chunkPos)); }
    [[nodiscard]] const_reference at(const Int3& chunkPos) const { return *(mChunks.at(chunkPos)); }

    reference operator[](const Int3& chunkPos) { return at(chunkPos); }
    const_reference operator[](const Int3& chunkPos) const { return at(chunkPos); }

    iterator insert(const Int3& chunkPos, data_t&& chunk) {
        mChunks[chunkPos] = std::move(chunk);
        return mChunks.find(chunkPos);
    }

    iterator erase(iterator it) { return mChunks.erase(it); }
    void erase(const Int3& chunkPos) { mChunks.erase(chunkPos); }

    iterator reset(iterator it, Chunk* chunk) {
        it->second.reset(chunk);
        return it;
    }

    iterator reset(const Int3& chunkPos, Chunk* chunk) { return reset(mChunks.find(chunkPos), chunk); }

    template <typename... ArgType, typename Func>
    void doIfLoaded(const Int3& chunkPos, Func func, ArgType&&... args) {
        auto iter = mChunks.find(chunkPos);
        if (iter != mChunks.end())
            func(*(iter->second), std::forward<ArgType>(args)...);
    };

    [[nodiscard]] bool isLoaded(const Int3& chunkPos) const noexcept { return mChunks.find(chunkPos) != mChunks.end(); }

    // Convert world position to chunk coordinate (one axis)
    static int getAxisPos(int pos) noexcept {
        return pos >> Chunk::SizeLog2();
    }

    // Convert world position to chunk coordinate (all axes)
    static Int3 getPos(const Int3& pos) noexcept {
        return Int3(getAxisPos(pos.X), getAxisPos(pos.Y), getAxisPos(pos.Z));
    }

    // Convert world position to block coordinate in chunk (one axis)
    static int getBlockAxisPos(int pos) noexcept { return pos & (Chunk::Size() - 1); }

    // Convert world position to block coordinate in chunk (all axes)
    static Int3 getBlockPos(const Int3& pos) noexcept {
        return Int3(getBlockAxisPos(pos.X), getBlockAxisPos(pos.Y), getBlockAxisPos(pos.Z));
    }

    // Get block data
    [[nodiscard]] Game::World::BlockData getBlock(const Int3& pos) const { return at(getPos(pos)).getBlock(getBlockPos(pos)); }

    // Set block data
    void setBlock(const Int3& pos, Game::World::BlockData block) { at(getPos(pos)).setBlock(getBlockPos(pos), block); }

private:
    array_t mChunks;
};

NWCOREAPI size_t NWAPICALL nwRegisterChunkGenerator(ChunkGenerator generator);