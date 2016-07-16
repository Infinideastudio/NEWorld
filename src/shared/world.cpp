/*
* NEWorld: A free game with similar rules to Minecraft.
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

#include "logger.h"
#include "world.h"
#include "chunk.h"

void World::expandChunkArray(size_t c)
{
    m_chunkCount += c;
    if (m_chunkCount > m_chunkArraySize)
    {
        m_chunkArraySize *= 2;
        auto newchunks = static_cast<Chunk**>(realloc(m_chunks, m_chunkArraySize * sizeof(Chunk*)));
        assert(newchunks != nullptr);
        if (newchunks == nullptr)
        {
            fatalstream << "Failed to create chunks, size:" << m_chunkArraySize;
            // TODO: exit
        }
        else m_chunks = newchunks;
    }
}

size_t World::getChunkIndex(const Vec3i& pos) const
{
    // Binary search
    assert(m_chunkCount);
    size_t first = 0, last = m_chunkCount - 1, middle;
    do
    {
        middle = (first + last) / 2;
        const Vec3i& curr = m_chunks[middle]->getPos();
        if (curr > pos)
            last = middle - 1;
        else if (curr < pos)
            first = middle + 1;
    }
    while (first <= last);
    return middle;
}

Chunk* World::addChunk(const Vec3i& chunkPos)
{
    int index = getChunkIndex(chunkPos);
    if (m_chunkCount && m_chunks[index]->getPos() == chunkPos)
    {
        assert(false);
        return nullptr;
    }
    newChunkPtr(index);
    m_chunks[index] = new Chunk(chunkPos);
    // TODO: Update chunk pointer cache
    // TODO: Update chunk pointer array
    // Return pointer
    return m_chunks[index];
}

int World::deleteChunk(const Vec3i& chunkPos)
{
    int index = getChunkIndex(chunkPos);
    if (m_chunkCount == 0 || m_chunks[index]->getPos() != chunkPos)
    {
        assert(false);
        return 1;
    }
    delete m_chunks[index];
    eraseChunkPtr(index);
    // Update chunk pointer array
    return 0;
}

void World::update()
{
}
