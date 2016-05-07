#include "world.h"

using std::abs;

int World::getChunkIndex(ChunkID id)
{
    // Binary search
    int first = 0, last = chunkCount - 1, middle;
    do
    {
        ChunkID cur = getChunkID(chunks[middle]->getX(), chunks[middle]->getY(), chunks[middle]->getZ);
        if (chunks[middle]->id > cid)
            last = middle - 1;
        else if (chunks[middle]->id < cid)
            first = middle + 1;
        middle = (first + last) / 2;
    }
    return middle;
}

World::World()
{

}

ChunkID getChunkID(int x, int y, int z)
{
    if (y == -WORLD_BOUND_Y)
        y = 0;
    if (y <= 0)
        y = abs(y) + (ChunkID(1) << WORLD_BOUND_Y_LOG2);

    if (x == -WORLD_BOUND_X)
        x = 0;
    if (x <= 0)
        x = abs(x) + (ChunkID(1) << WORLD_BOUND_X_LOG2);

    if (z == -WORLD_BOUND_Z)
        z = 0;
    if (z <= 0)
        z = abs(z) + (ChunkID(1) << WORLD_BOUND_Z_LOG2);

    return (ChunkID(x) << (WORLD_BOUND_Y_LOG2 + WORLD_BOUND_Z_LOG2)) ^ (ChunkID(y) << WORLD_BOUND_Z_LOG2) ^ ChunkID(z);
}

Chunk* World::getChunkPtr(int index)
{
//    assert(index >= 0 && index <= chunkCount);
    return chunks[index];
}

Chunk* World::getChunkPtr(int x, int y, int z)
{
    unsigned long long id = getChunkID(x, y, z);


}