#include "chunk.h"

Chunk::Chunk(Vec3 pos_) :pos(pos_)
{
}

Vec3 Chunk::getPos() const
{
    return pos;
}

BlockData Chunk::getBlock(Vec3 pos) const
{
    return blocks[pos.x*CHUNK_SIZE*CHUNK_SIZE + pos.y*CHUNK_SIZE + pos.z];
}

void Chunk::setBlock(Vec3 pos, BlockData block)
{
    blocks[pos.x*CHUNK_SIZE*CHUNK_SIZE + pos.y*CHUNK_SIZE + pos.z] = block;
}
