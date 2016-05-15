/*
* NEWorld: An free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WORLD_H_
#define WORLD_H_

#include <algorithm>
#include <string>
#include <cstring> // malloc, realloc, free
using std::abs;

#include "common.h"
#include "chunk.h"

class World
{
private:
    // All chunks (chunk array)
    Chunk** chunks;
    // Size of chunk array
    size_t chunkArraySize;
    // Loaded chunks count
    size_t chunkCount;

    // Expand chunk array
    void expandChunkArray(size_t expandCount);
    // Reduce chunk array
    void reduceChunkArray(size_t reduceCount);
    // New pointer at chunks[index]
    void newChunkPtr(size_t index);
    // Erase pointer at chunks[index]
    void eraseChunkPtr(size_t index);
    // Search chunk index, or the index the chunk should insert into
    size_t getChunkIndex(const Vec3i& chunkPos) const;

public:
    // World name
    std::string name;

    World();
    ~World();
    World(const World&) = delete;
    World& operator= (const World&) = delete;

    // Get chunk count
    size_t getChunkCount() const { return chunkCount; }
    // Add chunk
    Chunk* addChunk(const Vec3i& chunkPos);
    // Delete chunk
    int deleteChunk(const Vec3i& chunkPos);
    // Get chunk pointer by index
    Chunk* getChunkPtr(size_t index) const;
    // Get chunk pointer by chunk coordinates
    Chunk* getChunkPtr(const Vec3i& chunkPos) const;

    // Convert world position to chunk coordinate (one axis)
    int getChunkPos(int pos) const { return pos >> ChunkSizeLog2; }
    // Convert world position to chunk coordinate (all axes)
    Vec3i getChunkPos(const Vec3i& pos) const { return Vec3i(getChunkPos(pos.x), getChunkPos(pos.y), getChunkPos(pos.z)); }
    // Convert world position to block coordinate in chunk (one axis)
    int getBlockPos(int pos) const { return pos & (ChunkSize - 1); }
    // Convert world position to block coordinate in chunk (all axes)
    Vec3i getBlockPos(const Vec3i& pos) const { return Vec3i(getBlockPos(pos.x), getBlockPos(pos.y), getBlockPos(pos.z)); }

    // Get block data
    BlockData getBlock(const Vec3i& pos) const;
    // Get block reference
//    BlockData& getBlock(const Vec3i& pos);
    // Set block data
    void setBlock(const Vec3i& pos, BlockData block);

};

#endif // !WORLD_H_
