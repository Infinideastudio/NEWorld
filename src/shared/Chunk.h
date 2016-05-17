/*
* NEWorld: A free game with similar rules to Minecraft.
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

#ifndef CHUNK_H_
#define CHUNK_H_

#include "common.h"
#include "vec3.h"
#include "blockdata.h"

const int ChunkSizeLog2 = 5;
const int ChunkSize = 1 << ChunkSizeLog2; // 2 ^ ChunkSizeLog2 == 32

class Chunk
{
private:
    Vec3i m_pos;
    BlockData m_blocks[ChunkSize*ChunkSize*ChunkSize];

public:
    explicit Chunk(const Vec3i& chunkPos) :m_pos(chunkPos) {}

    // Get chunk position
    Vec3i getPos() const
    { return m_pos; }

    // Get block data in this chunk
    BlockData getBlock(const Vec3i& pos) const
    {
        assert(pos.x >= 0 && pos.x < ChunkSize && pos.y >= 0 && pos.y < ChunkSize && pos.z >= 0 && pos.z < ChunkSize);
        return m_blocks[pos.x*ChunkSize*ChunkSize + pos.y*ChunkSize + pos.z];
    }

    // Get block reference in this chunk
    BlockData& getBlock(const Vec3i& pos)
    {
        assert(pos.x >= 0 && pos.x < ChunkSize && pos.y >= 0 && pos.y < ChunkSize && pos.z >= 0 && pos.z < ChunkSize);
        return m_blocks[pos.x*ChunkSize*ChunkSize + pos.y*ChunkSize + pos.z];
    }

    // Set block data in this chunk
    void setBlock(const Vec3i& pos, BlockData block)
    {
        assert(pos.x >= 0 && pos.x < ChunkSize && pos.y >= 0 && pos.y < ChunkSize && pos.z >= 0 && pos.z < ChunkSize);
        m_blocks[pos.x*ChunkSize*ChunkSize + pos.y*ChunkSize + pos.z] = block;
    }

};

#endif // !CHUNK_H_
