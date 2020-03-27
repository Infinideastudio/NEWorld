// 
// Core: nwchunk.cpp
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

#include <Common/Logger.h>
#include "Common/Math/Vector.h"
#include "Game/World/Blocks.h"
#include "nwchunk.h"
#include "Game/World/world.h"

void NWAPICALL DefaultChunkGen(const ChunkGenerateArgs*) {
    // This is the default terrain generator. Use this when no generators were loaded from plugins.
    
    //for (int x = 0; x < chunk::Size() * chunk::Size() * chunk::Size(); ++x)
    //    blocks[x] = BlockData(0, daylightBrightness, 0);
}

bool Chunk::ChunkGeneratorLoaded = false;
ChunkGenerator Chunk::ChunkGen = DefaultChunkGen;

Chunk::Chunk(const Vec3i& position, const class World& world, LoadBehavior behavior)
    : mPosition(position), mWorld(&world) {
    if (behavior == LoadBehavior::Build) build(mWorld->getDaylightBrightness());
    else mLoading = true;
}

Chunk::Chunk(const Vec3i& position, const class World& world, const ChunkDataStorageType& data)
    : mPosition(position), mWorld(&world) {
    replaceChunk(data);
}

void Chunk::replaceChunk(const ChunkDataStorageType& data) noexcept {
    if(data.size()==1) { // Monotonic chunk
        setMonotonic(Game::World::BlockData(data[0]));
        return;
    }

    assert(data.size() == Chunk::BlocksSize);
    allocateBlocks();
    static_assert(std::is_trivially_copyable<Game::World::BlockData>::value);
    std::memcpy(getBlocks()->data(), data.data(), sizeof(Game::World::BlockData) * BlocksSize);
    mLoading = false;
}

Chunk::ChunkDataStorageType Chunk::getChunkForExport() const noexcept {
    if (isMonotonic())
        return std::vector<uint32_t> { getMonotonicBlock().getData() };

    std::vector<uint32_t> chunkData;
    chunkData.resize(BlocksSize);
    std::memcpy(chunkData.data(), getBlocks()->data(), sizeof(Game::World::BlockData) * Chunk::BlocksSize);
    return chunkData;
}

void Chunk::build(int daylightBrightness) {
    ChunkGenerateArgs args { &getPosition(), this, daylightBrightness };
    ChunkGen(&args);
    setUpdated(true);
}

size_t nwRegisterChunkGenerator(const ChunkGenerator generator) {
    if (Chunk::ChunkGeneratorLoaded) {
        warningstream << "Ignoring multiple chunk generators!";
        return 1;
    }
    Chunk::ChunkGeneratorLoaded = true;
    Chunk::ChunkGen = generator;
    debugstream << "Registered chunk generator";
    return 0;
}