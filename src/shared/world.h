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

#include "common.h"
#include "chunk.h"

class World
{
private:
    // All chunks (chunk array)
    Chunk** chunks;
    // Size of chunk array
    int chunkArraySize;
    // Loaded chunks count
    int chunkCount;

    // Expand chunk array
    void expandChunkArray(int expandCount);
    // Reduce chunk array
    void reduceChunkArray(int reduceCount);
    // New pointer at chunks[index]
    void newChunkPtr(int index);
    // Erase pointer at chunks[index]
    void eraseChunkPtr(int index);
    // Search chunk index, or the index the chunk should insert into
    int getChunkIndex(const Vec3& chunkPos) const;

public:
    // World name
    std::string name;

    World();
    ~World();

    // Release resources
    ~World();

    // Add chunk
    Chunk* addChunk(const Vec3& chunkPos);
    // Delete chunk
    int deleteChunk(const Vec3& chunkPos);
    // Get chunk pointer by index
    Chunk* getChunkPtr(int index) const;
    // Get chunk pointer by chunk coordinates
    Chunk* getChunkPtr(const Vec3& chunkPos) const;

    // Convert world position to chunk coordinate (one axis)
    int getChunkPos(int pos) const { return pos >> ChunkSizeLog2; }
    // Convert world position to chunk coordinate (all axes)
    Vec3 getChunkPos(const Vec3& pos) const { return Vec3(getChunkPos(pos.x), getChunkPos(pos.y), getChunkPos(pos.z)); }
    // Convert world position to block coordinate in chunk (one axis)
    int getBlockPos(int pos) const { return pos & (ChunkSize - 1); }
    // Convert world position to block coordinate in chunk (all axes)
    Vec3 getBlockPos(const Vec3& pos) const { return Vec3(getBlockPos(pos.x), getBlockPos(pos.y), getBlockPos(pos.z)); }
    
    // Get block data
    BlockData getBlock(const Vec3& pos) const;
    // Set block data
    void setBlock(const Vec3& pos, BlockData block);

};

#endif // !WORLD_H_
