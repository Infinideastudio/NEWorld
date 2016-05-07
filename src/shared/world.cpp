#include "world.h"
#include "chunk.h"

using std::abs;

ChunkID World::getChunkID(Vec3 chunkPos)
{
    int x = chunkPos.x, y = chunkPos.y, z = chunkPos.z;
    if (y == -WorldBoundY)
        y = 0;
    if (y <= 0)
        y = abs(y) + WorldBoundY;

    if (x == -WorldBoundX)
        x = 0;
    if (x <= 0)
        x = abs(x) + WorldBoundX;

    if (z == -WorldBoundZ)
        z = 0;
    if (z <= 0)
        z = abs(z) + WorldBoundZ;

    return (ChunkID(x) << (WorldBoundYLog2 + WorldBoundZLog2)) ^ (ChunkID(y) << WorldBoundZLog2) ^ ChunkID(z);
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

World::World()
{
    // Initialize world
}

Chunk * World::AddChunk(Vec3 chunkPos)
{
    ChunkID id = getChunkID(chunkPos);
    int index = getChunkIndex(id);
    if (chunkCount && getChunkID(chunks[index]->getPos()) == id)
    {
        // Error: chunk already exist
    }
    // Append chunk Array
    // Move elements to get slot
    // Insert into slot
    // Update chunk pointer chache
    // Update chunk pointer array
    // Return pointer
    return chunks[index];
}

int World::DeleteChunk(Vec3 chunkPos)
{
    ChunkID id = getChunkID(chunkPos);
    int index = getChunkIndex(id);
    if (!chunkCount || getChunkID(chunks[index]->getPos()) != id)
    {
        // Error: chunk isn't exist
    }
    delete chunks[index];
    // Move elements to overwrite
    // Reduce chunk array
    // Update chunk pointer array
    return 0;
}

Chunk* World::getChunkPtr(int index)
{
//    assert(index >= 0 && index <= chunkCount);
    return chunks[index];
}

Chunk* World::getChunkPtr(Vec3 chunkPos)
{
//    assert(index >= 0 && index <= chunkCount);
    // Try chunk pointer cache
    // Try chunk pointer array
    return chunks[getChunkIndex(getChunkID(chunkPos))];
    // Update chunk pointer array
    // Update chunk pointer cache
}

BlockData World::getBlock(Vec3 pos)
{
    Chunk* chunk = getChunkPtr(getChunkPos(pos));
//    assert(chunk != nullptr);
    return chunk->getBlock(getBlockPos(pos));
}

void World::setBlock(Vec3 pos, BlockData block)
{
    Chunk* chunk = getChunkPtr(getChunkPos(pos));
//    assert(chunk != nullptr);
    chunk->setBlock(getBlockPos(pos), block);
}
