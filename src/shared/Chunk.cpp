#include "chunk.h"

BlockData Chunk::getBlock(int x, int y, int z) const
{
    return blocks[x*CHUNK_SIZE*CHUNK_SIZE + y*CHUNK_SIZE + z];
}

void Chunk::setBlock(int x, int y, int z, BlockData block)
{
    blocks[x*CHUNK_SIZE*CHUNK_SIZE + y*CHUNK_SIZE + z] = block;
}
