#ifndef CHUNK_H_
#define CHUNK_H_

#include "blockdata.h"
#include "aabb.h"

const int CHUNK_SIZE = 32;

class Chunk
{
    private:
        BlockData blocks[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];

    public:
        // Get block data in this chunk
        BlockData getBlock(int x, int y, int z) const;
        // Set block data in this chunk
        void setBlock(int x, int y, int z, BlockData block);

};

#endif // !CHUNK_H_
