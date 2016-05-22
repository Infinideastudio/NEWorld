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
#include "../shared/world.h"
#include "../shared/chunkpointerarray.h"

constexpr int MaxChunkLoadCount = 64;
constexpr int MaxChunkUnloadCount = 64;

class WorldLoader
{
private:
    World* m_world; // World
    ChunkPointerArray* m_cpa; // ChunkPointerArray used to detect unloaded chunks in load range
    int m_chunkLoadCount, m_chunkUnloadCount, m_loadRange;
    pair<Vec3i, int> m_chunkLoadList[256]; // Chunk load list <position, distance>
    pair<Chunk*, int> m_chunkUnloadList[256]; // Chunk unload list <pointer, distance>

public:
    explicit WorldLoader(World* world, ChunkPointerArray* cpa)
        :m_world(world), m_cpa(cpa), m_chunkLoadCount(0), m_chunkUnloadCount(0) {}

    // Set load range
    void setLoadRange(int x)
    { m_loadRange = x; }
    // Find the nearest chunks in load range to load, fartherest chunks out of load range to unload
    void sortChunkLoadUnloadList(const Vec3i& centerPos);
    // Load & unload chunks
    void loadUnloadChunks();

};

#endif // !WORLDLOADER_H_
