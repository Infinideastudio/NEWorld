#ifndef CHUNK_H_
#define CHUNK_H_

#include "blockdata.h"
#include "vec3.h"

const int CHUNK_SIZE = 32;

class Chunk
{
private:
    Vec3 pos;
    BlockData blocks[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];

public:
    Chunk(Vec3 chunk_pos);

    // Get chunk position
    Vec3 getPos() const;
    // Get block data in this chunk
    BlockData getBlock(Vec3 pos) const;
    // Set block data in this chunk
    void setBlock(Vec3 pos, BlockData block);

};

#endif // !CHUNK_H_
