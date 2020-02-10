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
#include "Blocks.h"
#include "Common/Debug.h"
#include "Common/Math/Vector.h"
#include "Common/Utility.h"

class Chunk;
using ChunkGenerator = std::add_pointer_t<void(const Vec3i*, Chunk*, int)>;

class NWCOREAPI Chunk final : public NonCopyable {
public:
    // Chunk size
    static bool ChunkGeneratorLoaded;
    static ChunkGenerator ChunkGen;
    static constexpr int BlocksSize = 32 * 32 * 32;
    static constexpr int SizeLog2() { return 5; }
    static constexpr int Size() { return 32; };
    using Blocks = std::array<BlockData, BlocksSize>;
    using ChunkDataStorageType = std::vector<uint32_t>;

    enum class LoadBehavior { Build, Loading };

    explicit Chunk(const Vec3i& position, const class World& world, LoadBehavior behavior = LoadBehavior::Build);
    explicit Chunk(const Vec3i& position, const class World& world, const ChunkDataStorageType& data);
    ~Chunk() = default;

    // Get chunk position
    const Vec3i& getPosition() const noexcept { return mPosition; }

    // Get chunk updated flag
    bool isUpdated() const noexcept { return mUpdated; }

    bool isModified() const noexcept { return mModified; }

    // Get chunk loading status
    bool isLoading() const noexcept { return mLoading; }
    void replaceChunk(const ChunkDataStorageType& data) noexcept;

    // Set chunk updated flag
    void setUpdated(bool updated) const noexcept { mUpdated = updated; }

    // Get block data in this chunk
    BlockData getBlock(const Vec3i& pos) const {
        Assert(pos.x >= 0 && pos.x < Size() && pos.y >= 0 && pos.y < Size() && pos.z >= 0 && pos.z < Size());
        return !isMonotonic() ? (*getBlocks())[pos.x * Size() * Size() + pos.y * Size() + pos.z] : mMonotonicBlock;
    }

    // Get block pointer. Note that they might return nullptr in case of monotonic chunk.
    Blocks* getBlocks() noexcept { return mBlocks.get(); }
    const Blocks* getBlocks() const noexcept { return mBlocks.get(); }
    ChunkDataStorageType getChunkForExport() const noexcept;

    // Set block data in this chunk
    void setBlock(const Vec3i& pos, BlockData block) {
        Assert(pos.x >= 0 && pos.x < Size() && pos.y >= 0 && pos.y < Size() && pos.z >= 0 && pos.z < Size());
        if (isMonotonic() && block != mMonotonicBlock) allocateBlocks();
        (*getBlocks())[pos.x * Size() * Size() + pos.y * Size() + pos.z] = block;
        mUpdated = true;
    }

    // Build chunk
    void build(int daylightBrightness);

    // Reference Counting
    void markRequest() noexcept {
        std::unique_lock<std::mutex> lock(mMutex);
        mLastRequestTime = std::chrono::steady_clock::now();
    }

    void increaseRef() noexcept {
        std::unique_lock<std::mutex> lock(mMutex);
        ++mReferenceCount;
    }

    void decreaseRef() noexcept {
        std::unique_lock<std::mutex> lock(mMutex);
        --mReferenceCount;
    }

    bool checkReleaseable() noexcept {
        std::unique_lock<std::mutex> lock(mMutex);
        using namespace std::chrono;
        return (((steady_clock::now() - mLastRequestTime) > 10s) && mReferenceCount <= 0);
    }

    const World* getWorld() const noexcept { return mWorld; }
    bool isMonotonic() const noexcept { return mBlocks == nullptr; }
    BlockData getMonotonicBlock() const noexcept { return mMonotonicBlock; }
    void allocateBlocks() { /*Assert(isMonotonic());*/
        if (!isMonotonic()) return;
        mBlocks = std::make_unique<Blocks>();
        // TODO: fill the blocks with the monotonic block.
    }
    void setMonotonic(BlockData block) noexcept { Assert(isMonotonic()); mMonotonicBlock = block; }

private:
    std::mutex mMutex;
    Vec3i mPosition;
    
    std::unique_ptr<Blocks> mBlocks;
    BlockData mMonotonicBlock;

    // TODO: somehow avoid it! not safe.
    mutable bool mUpdated = false;
    bool mModified = false;
    bool mLoading = false;
    const class World* mWorld;
    // For Garbage Collection
    int mReferenceCount{0};
    std::chrono::steady_clock::time_point mLastRequestTime;
};


struct ChunkHasher {
    constexpr size_t operator()(const Vec3i& t) const noexcept {
        return static_cast<size_t>(t.x * 23947293731 + t.z * 3296467037 + t.y * 1234577);
    }
};

struct NWCOREAPI ChunkOnReleaseBehavior {
    enum class Behavior : size_t {
        Release,
        DeReference
    } status;

    void operator()(Chunk* target) const {
        switch (status) {
        case Behavior::Release:
            delete target;
            break;
        case Behavior::DeReference:
            target->decreaseRef();
            break;
        }
    }

    constexpr ChunkOnReleaseBehavior() : status(Behavior::Release) {};
    constexpr ChunkOnReleaseBehavior(Behavior b) : status(b) {}
};

class ChunkManager : public NonCopyable {
public:
    using data_t = std::unique_ptr<Chunk, ChunkOnReleaseBehavior>;
    using array_t = std::unordered_map<Vec3i, data_t, ChunkHasher>;
    using iterator = array_t::iterator;
    using const_iterator = array_t::const_iterator;
    using reference = Chunk&;
    using const_reference = const Chunk&;
    ChunkManager() = default;
    ChunkManager(size_t size) { mChunks.reserve(size); }
    ChunkManager(ChunkManager&& rhs) noexcept : mChunks(std::move(rhs.mChunks)) {}
    ~ChunkManager() = default;
    // Access and modifiers
    size_t size() const noexcept { return mChunks.size(); }
    iterator begin() noexcept { return mChunks.begin(); }
    iterator end() noexcept { return mChunks.end(); }
    const_iterator begin() const noexcept { return mChunks.cbegin(); }
    const_iterator end() const noexcept { return mChunks.cend(); }

    reference at(const Vec3i& chunkPos) { return *(mChunks.at(chunkPos)); }
    const_reference at(const Vec3i& chunkPos) const { return *(mChunks.at(chunkPos)); }

    reference operator[](const Vec3i& chunkPos) { return at(chunkPos); }
    const_reference operator[](const Vec3i& chunkPos) const { return at(chunkPos); }

    iterator insert(const Vec3i& chunkPos, data_t&& chunk) {
        mChunks[chunkPos] = std::move(chunk);
        return mChunks.find(chunkPos);
    }

    iterator erase(iterator it) { return mChunks.erase(it); }
    void erase(const Vec3i& chunkPos) { mChunks.erase(chunkPos); }

    iterator reset(iterator it, Chunk* chunk) {
        it->second.reset(chunk);
        return it;
    }

    iterator reset(const Vec3i& chunkPos, Chunk* chunk) { return reset(mChunks.find(chunkPos), chunk); }

    template <typename... ArgType, typename Func>
    void doIfLoaded(const Vec3i& chunkPos, Func func, ArgType&&... args) {
        auto iter = mChunks.find(chunkPos);
        if (iter != mChunks.end())
            func(*(iter->second), std::forward<ArgType>(args)...);
    };

    bool isLoaded(const Vec3i& chunkPos) const noexcept { return mChunks.find(chunkPos) != mChunks.end(); }

    // Convert world position to chunk coordinate (one axis)
    static int getAxisPos(int pos) noexcept {
        return pos >> Chunk::SizeLog2();
    }

    // Convert world position to chunk coordinate (all axes)
    static Vec3i getPos(const Vec3i& pos) noexcept {
        return Vec3i(getAxisPos(pos.x), getAxisPos(pos.y), getAxisPos(pos.z));
    }

    // Convert world position to block coordinate in chunk (one axis)
    static int getBlockAxisPos(int pos) noexcept { return pos & (Chunk::Size() - 1); }

    // Convert world position to block coordinate in chunk (all axes)
    static Vec3i getBlockPos(const Vec3i& pos) noexcept {
        return Vec3i(getBlockAxisPos(pos.x), getBlockAxisPos(pos.y), getBlockAxisPos(pos.z));
    }

    // Get block data
    BlockData getBlock(const Vec3i& pos) const { return at(getPos(pos)).getBlock(getBlockPos(pos)); }

    // Set block data
    void setBlock(const Vec3i& pos, BlockData block) { at(getPos(pos)).setBlock(getBlockPos(pos), block); }

private:
    array_t mChunks;
};

NWCOREAPI size_t NWAPICALL nwRegisterChunkGenerator(ChunkGenerator generator);