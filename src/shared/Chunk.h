#ifndef CHUNK_H_
#define CHUNK_H_

#include "blockdata.h"
#include "Common.h"

const int chunkSize = 32;

ChunkID getChunkID(Vec3 chunkPos);

class Chunk
{
private:
    const Vec3 pos;
    const ChunkID cid;
    BlockData blocks[chunkSize*chunkSize*chunkSize];

public:
    Chunk(Vec3 chunkPos) :pos(chunkPos),cid(getChunkID(chunkPos)) {}

    // Get chunk position
    Vec3 getPos() const { return pos; }
    // Get block data in this chunk
    BlockData getBlock(Vec3 pos) const { return blocks[pos.x*chunkSize*chunkSize + pos.y*chunkSize + pos.z]; }
    // Set block data in this chunk
    void setBlock(Vec3 pos, BlockData block) { blocks[pos.x*chunkSize*chunkSize + pos.y*chunkSize + pos.z] = block; }
    // Get chunk's id
    ChunkID getID() const { return cid; }

};

#endif // !CHUNK_H_
