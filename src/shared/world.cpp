/*
* NEWorld: An free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "world.h"
#include "chunk.h"

void World::expandChunkArray(size_t c)
{
    chunkCount += c;
    if (chunkCount > chunkArraySize)
    {
        chunkArraySize <<= 1;
        chunks = (Chunk**)realloc(chunks, chunkArraySize * sizeof(Chunk*));
        assert(chunks != NULL);
    }
}

void World::reduceChunkArray(size_t c)
{
    assert(chunkCount >= c);
    chunkCount -= c;
}

void World::newChunkPtr(size_t index)
{
    expandChunkArray(1);
    for (size_t i = chunkCount - 1; i > index; i--)
        chunks[i] = chunks[i - 1];
    chunks[index] = nullptr;
}

void World::eraseChunkPtr(size_t index)
{
    for (size_t i = index; i < chunkCount - 1; i++)
        chunks[i] = chunks[i + 1];
    reduceChunkArray(1);
}

size_t World::getChunkIndex(const Vec3i& pos) const
{
    // Binary search
    assert(chunkCount);
    size_t first = 0, last = chunkCount - 1, middle;
    do
    {
        middle = (first + last) / 2;
        Vec3i& curr = chunks[middle]->getPos();
        if (curr > pos)
            last = middle - 1;
        else if (curr < pos)
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

World::~World()
{
}

Chunk* World::addChunk(const Vec3i& chunkPos)
{
    int index = getChunkIndex(chunkPos);
    if (chunkCount && chunks[index]->getPos() == chunkPos)
    {
        assert(false);
        return nullptr;
    }
    newChunkPtr(index);
    chunks[index] = new Chunk(chunkPos);
    // Update chunk pointer cache
    // Update chunk pointer array
    // Return pointer
    return chunks[index];
}

int World::deleteChunk(const Vec3i& chunkPos)
{
    int index = getChunkIndex(chunkPos);
    if (chunkCount == 0 || chunks[index]->getPos() != chunkPos)
    {
        assert(false);
        return 1;
    }
    delete chunks[index];
    eraseChunkPtr(index);
    // Update chunk pointer array
    return 0;
}

Chunk* World::getChunkPtr(size_t index) const
{
    if (index >= chunkCount)
    {
        assert(false);
        return nullptr;
    }
    return chunks[index];
}

Chunk* World::getChunkPtr(const Vec3i& chunkPos) const
{
    // TODO: Try chunk pointer cache
    // TODO: Try chunk pointer array
    Chunk* res = chunks[getChunkIndex(chunkPos)];
    // TODO: Update chunk pointer array
    // TODO: Update chunk pointer cache
    return res;
}

BlockData World::getBlock(const Vec3i& pos) const
{
    Chunk* chunk = getChunkPtr(getChunkPos(pos));
    if (chunk == nullptr) {

    }
    return chunk->getBlock(getBlockPos(pos));
}

void World::setBlock(const Vec3i& pos, BlockData block)
{
    Chunk* chunk = getChunkPtr(getChunkPos(pos));
    assert(chunk != nullptr);
    chunk->setBlock(getBlockPos(pos), block);
}
