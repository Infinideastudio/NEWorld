#ifndef CHUNK_H_
#define CHUNK_H_

#include "blockdata.h"
#include "vec3.h"

const int chunkSize = 32;

class Chunk
{
private:
    Vec3 pos;
    BlockData blocks[chunkSize*chunkSize*chunkSize];

public:
    Chunk(Vec3 chunk_pos) :pos(chunk_pos) {}

    // Get chunk position
    Vec3 getPos() const { return pos; }
    // Get block data in this chunk
    BlockData getBlock(Vec3 pos) const { return blocks[pos.x*chunkSize*chunkSize + pos.y*chunkSize + pos.z]; }
    // Set block data in this chunk
    void setBlock(Vec3 pos, BlockData block) { blocks[pos.x*chunkSize*chunkSize + pos.y*chunkSize + pos.z] = block; }

};

#endif // !CHUNK_H_
