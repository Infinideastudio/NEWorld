#include "world.h"
#include "chunk.h"

using std::abs;

void World::expandChunkArray(int c)
{
    chunkCount += c;
    if (chunkCount > chunkArraySize) {
        chunkArraySize *= 2;
        chunks = (Chunk**)realloc(chunks, chunkArraySize * sizeof(Chunk*));
//        assert(!chunkCount || chunks);
    }
}

void World::reduceChunkArray(int c)
{
    chunkCount -= c;
//    assert(chunkCount >= 0);
}

void World::newChunkPtr(int index)
{
    expandChunkArray(1);
    for (int i = chunkCount - 1; i > index; i--)
        chunks[i] = chunks[i - 1];
    chunks[index] = nullptr;
}

void World::eraseChunkPtr(int index)
{
    for (int i = index; i < chunkCount - 1; i++)
        chunks[i] = chunks[i + 1];
    reduceChunkArray(1);
}

ChunkID World::getChunkID(const Vec3& chunkPos) const
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

int World::getChunkIndex(ChunkID id) const
{
    // Binary search
//    assert(chunkCount);
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
    chunkArraySize = 1024;
    chunks = (Chunk**)malloc(chunkArraySize * sizeof(Chunk*));

}

Chunk* World::addChunk(const Vec3& chunkPos)
{
    ChunkID id = getChunkID(chunkPos);
    int index = getChunkIndex(id);
    if (chunkCount && getChunkID(chunks[index]->getPos()) == id)
    {
        // Error: chunk already exist
    }
    newChunkPtr(index);
    chunks[index] = new Chunk(chunkPos);
    // Update chunk pointer cache
    // Update chunk pointer array
    // Return pointer
    return chunks[index];
}

int World::deleteChunk(const Vec3& chunkPos)
{
    ChunkID id = getChunkID(chunkPos);
    int index = getChunkIndex(id);
    if (!chunkCount || getChunkID(chunks[index]->getPos()) != id)
    {
        // Error: chunk isn't exist
    }
    delete chunks[index];
    eraseChunkPtr(index);
    // Update chunk pointer array
    return 0;
}

Chunk* World::getChunkPtr(int index) const
{
//    assert(index >= 0 && index <= chunkCount);
    return chunks[index];
}

Chunk* World::getChunkPtr(const Vec3& chunkPos) const
{
//    assert(index >= 0 && index <= chunkCount);
    // Try chunk pointer cache
    // Try chunk pointer array
    return chunks[getChunkIndex(getChunkID(chunkPos))];
    // Update chunk pointer array
    // Update chunk pointer cache
}

BlockData World::getBlock(const Vec3& pos) const
{
    Chunk* chunk = getChunkPtr(getChunkPos(pos));
//    assert(chunk != nullptr);
    return chunk->getBlock(getBlockPos(pos));
}

void World::setBlock(const Vec3& pos, BlockData block)
{
    Chunk* chunk = getChunkPtr(getChunkPos(pos));
//    assert(chunk != nullptr);
    chunk->setBlock(getBlockPos(pos), block);
}
