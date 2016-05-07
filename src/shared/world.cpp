#include "world.h"

using std::abs;

int World::getChunkIndex(ChunkID id)
{
    // Binary search
    int first = 0, last = chunkCount - 1, middle;
    do
    {
        ChunkID cur = getChunkID(chunks[middle]->getPos());
        if (chunks[middle]->id > cid)
            last = middle - 1;
        else if (chunks[middle]->id < cid)
            first = middle + 1;
        middle = (first + last) / 2;
    }
    return middle;
}

ChunkID getChunkID(int x, int y, int z)
{
    if (y == -worldBoundY)
        y = 0;
    if (y <= 0)
        y = abs(y) + (ChunkID(1) << worldBoundYLog2);

    if (x == -worldBoundX)
        x = 0;
    if (x <= 0)
        x = abs(x) + (ChunkID(1) << worldBoundXLog2);

    if (z == -worldBoundZ)
        z = 0;
    if (z <= 0)
        z = abs(z) + (ChunkID(1) << worldBoundZLog2);

    return (ChunkID(x) << (worldBoundYLog2 + worldBoundZLog2)) ^ (ChunkID(y) << worldBoundZLog2) ^ ChunkID(z);
}

Chunk* World::getChunkPtr(int index)
{
    //assert(index >= 0 && index <= chunkCount);
    return chunks[index];
}

Chunk* World::getChunkPtr(Vec3 chunkPos)
{
    return getChunkPtr(getChunkIndex(getChunkID(chunkPos)));
}