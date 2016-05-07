#ifndef CHUNK_H_
#define CHUNK_H_

#include "blockdata.h"
#include "Common.h"

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
