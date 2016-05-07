#ifndef CHUNK_H_
#define CHUNK_H_

#include "BlockData.h"
#include "AABB.h"

class Chunk
{
public:
    // Get block data in this chunk
    BlockData getBlock(int x, int y, int z) const
    {
        return blocks[x*chunkSize*chunkSize + y*chunkSize + z];
    }
    // Set block data in this chunk
    void setBlock(int x, int y, int z, BlockData block)
    {
        blocks[x*chunkSize*chunkSize + y*chunkSize + z] = block;
    }


    static const int chunkSize = 32;

private:
    BlockData blocks[chunkSize*chunkSize*chunkSize];

};

#endif // !CHUNK_H_
