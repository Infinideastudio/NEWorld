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
#include "world.h"
using std::pair;

const int MaxChunkLoadCount = 64;
const int MaxChunkUnloadCount = 64;

class WorldLoader
{
private:
    World* m_world;
    int m_chunkLoadCount, m_chunkUnloadCount;
    pair<Vec3i, int> m_chunkLoadList[256]; // Chunk load list <position, distance>
    pair<Chunk*, int> m_chunkUnloadList[256]; // Chunk unload list <pointer, distance>

public:
    WorldLoader(World* _world_) :m_world(_world_) {}

    // Find the nearest chunks in load distance to load, fartherest chunks out of load distance to unload
    void sortChunkLoadUnloadList(const Vec3i& centerPos);
    // Load & Unload chunks
    void loadUnloadChunks();

};

#endif // !WORLDLOADER_H_
