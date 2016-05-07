#ifndef CHUNK_H_
#define CHUNK_H_

#include "blockdata.h"
#include "Common.h"

const int ChunkSize = 32;

class Chunk
{
private:
    const Vec3 pos;
    BlockData blocks[ChunkSize*ChunkSize*ChunkSize];

public:
    Chunk(Vec3 chunkPos) :pos(chunkPos)
    {
    }

    // Get chunk position
    Vec3 getPos() const { return pos; }
    // Get block data in this chunk
    BlockData getBlock(Vec3 pos) const { return blocks[pos.x*ChunkSize*ChunkSize + pos.y*ChunkSize + pos.z]; }
    // Set block data in this chunk
    void setBlock(Vec3 pos, BlockData block) { blocks[pos.x*ChunkSize*ChunkSize + pos.y*ChunkSize + pos.z] = block; }

};

#endif // !CHUNK_H_
