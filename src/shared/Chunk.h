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

#ifndef CHUNK_H_
#define CHUNK_H_

#include "blockdata.h"
#include "common.h"

const int ChunkSizeLog2 = 5;
const int ChunkSize = 1 << ChunkSizeLog2; // 2 ^ ChunkSizeLog2

class Chunk
{
private:
    Vec3 pos;
    BlockData blocks[ChunkSize*ChunkSize*ChunkSize];

public:
    Chunk(const Vec3& chunkPos) :pos(chunkPos) {}

    // Get chunk position
    Vec3 getPos() const { return pos; }
    // Get block data in this chunk
    BlockData getBlock(const Vec3& pos) const { return blocks[pos.x*ChunkSize*ChunkSize + pos.y*ChunkSize + pos.z]; }
    // Set block data in this chunk
    void setBlock(const Vec3& pos, BlockData block) { blocks[pos.x*ChunkSize*ChunkSize + pos.y*ChunkSize + pos.z] = block; }

};

#endif // !CHUNK_H_
