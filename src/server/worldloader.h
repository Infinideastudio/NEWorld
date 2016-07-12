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

#ifndef WORLDLOADER_H_
#define WORLDLOADER_H_

#include <utility>
using std::pair;
#include <world.h>
#include <chunkpointerarray.h>

constexpr int MaxChunkLoadCount = 64, MaxChunkUnloadCount = 64;

class WorldLoader
{
private:
    /// World
    World* m_world;
    /// ChunkPointerArray used to detect unloaded chunks in load range
    ChunkPointerArray* m_cpa;
    int m_chunkLoadCount, m_chunkUnloadCount, m_loadRange;
    /// Chunk load list [position, distance]
    pair<Vec3i, int> m_chunkLoadList[MaxChunkLoadCount];
    /// Chunk unload list [pointer, distance]
    pair<Chunk*, int> m_chunkUnloadList[MaxChunkUnloadCount];

public:
    WorldLoader(World* world, ChunkPointerArray* cpa)
        : m_world(world), m_cpa(cpa), m_chunkLoadCount(0), m_chunkUnloadCount(0), m_loadRange(0)
    {
    }

    /// Set load range
    void setLoadRange(int x)
    {
        m_loadRange = x;
    }

    /// Find the nearest chunks in load range to load, fartherest chunks out of load range to unload
    void sortChunkLoadUnloadList(const Vec3i& centerPos);
    /// Load & unload chunks
    void loadUnloadChunks();
};

#endif // !WORLDLOADER_H_
