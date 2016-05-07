#include "world.h"
#include "chunk.h"
using std::abs;

ChunkID getChunkID(Vec3 chunkPos)
{
    int x = chunkPos.x, y = chunkPos.y, z = chunkPos.z;
    if (y == -worldBoundY)
        y = 0;
    if (y <= 0)
        y = abs(y) + worldBoundY;

    if (x == -worldBoundX)
        x = 0;
    if (x <= 0)
        x = abs(x) + worldBoundX;

    if (z == -worldBoundZ)
        z = 0;
    if (z <= 0)
        z = abs(z) + worldBoundZ;

    return (ChunkID(x) << (worldBoundYLog2 + worldBoundZLog2)) ^ (ChunkID(y) << worldBoundZLog2) ^ ChunkID(z);
}

int World::getChunkIndex(ChunkID id)
{
    // Binary search
    int first = 0, last = chunkCount - 1, middle;
    do
    {
        middle = (first + last) / 2;
        ChunkID cur = getChunkID(chunks[middle]->getPos());
        if (cur > id)
            last = middle - 1;
        else if (cur < id)
            first = middle + 1;
    }
    while (first <= last);
    return middle;
}

// Initializes world
World::World()
{

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