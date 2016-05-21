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

#include "worldloader.h"

void WorldLoader::sortChunkLoadUnloadList(const Vec3i& centerPos)
{
    Vec3i centerCPos;
    Chunk* curChunk;
    int pl = 0, pu = 0;
    int distsqr, first, middle, last;

    // centerPos to chunk coords
    centerCPos = m_world->getChunkPos(centerPos);

    for (size_t ci = 0; ci < m_world->getChunkCount(); ci++)
    {
        Vec3i curPos = m_world->getChunkPtr(ci)->getPos();
        // Get chunk center pos
        curPos.x = curPos.x*ChunkSize + ChunkSize / 2 - 1;
        curPos.y = curPos.y*ChunkSize + ChunkSize / 2 - 1;
        curPos.z = curPos.z*ChunkSize + ChunkSize / 2 - 1;

        // Out of load range, pending to unload
        if (centerPos.chebyshevDistance(curPos) > m_loadRange)
        {
            // Distance from centerPos
            distsqr = (curPos - centerPos).lengthSqr();

            // Binary search in unload list
            first = 0;
            last = pl - 1;
            while (first <= last)
            {
                middle = (first + last) >> 1;
                if (distsqr > m_chunkUnloadList[middle].second)
                    last = middle - 1;
                else
                    first = middle + 1;
            }

            // Not very far, don't unload now
            if (first > pl || first >= MaxChunkUnloadCount) continue;

            // Move elements to make place
            for (int j = MaxChunkUnloadCount - 1; j > first; j--)
                m_chunkUnloadList[j] = m_chunkUnloadList[j - 1];

            // Insert into list
            m_chunkUnloadList[first].first = m_world->getChunkPtr(ci);
            m_chunkUnloadList[first].second = distsqr;

            // Add counter
            if (pl < MaxChunkUnloadCount) pl++;
        }
    }
    m_chunkUnloadCount = pl;

    for (int x = centerPos.x - m_loadRange; x <= centerPos.x + m_loadRange; x++)
        for (int y = centerPos.y - m_loadRange; y <= centerPos.y + m_loadRange; y++)
            for (int z = centerPos.z - m_loadRange; z <= centerPos.z + m_loadRange; z++)
                // In load range, pending to load
                if (/*cpArray.getChunkPtr(cx, cy, cz) == nullptr*/ true /* NOT FINISHED */)
                {
                    Vec3i curPos;
                    // Get chunk center pos
                    curPos.x = x*ChunkSize + ChunkSize / 2 - 1;
                    curPos.y = y*ChunkSize + ChunkSize / 2 - 1;
                    curPos.z = z*ChunkSize + ChunkSize / 2 - 1;

                    // Distance from centerPos
                    distsqr = (curPos - centerPos).lengthSqr();

                    // Binary search in load list
                    first = 0;
                    last = pu - 1;
                    while (first <= last)
                    {
                        middle = (first + last) >> 1;
                        if (distsqr < m_chunkLoadList[middle].second)
                            last = middle - 1;
                        else
                            first = middle + 1;
                    }

                    // Not very near, don't load now
                    if (first > pu || first >= MaxChunkLoadCount) continue;

                    // Move elements to make place
                    for (int j = MaxChunkLoadCount - 1; j > first; j--)
                        m_chunkLoadList[j] = m_chunkLoadList[j - 1];

                    // Insert into list
                    m_chunkLoadList[first].first = Vec3i(x, y, z);
                    m_chunkLoadList[first].second = distsqr;

                    // Add counter
                    if (pu < MaxChunkLoadCount) pu++;
                }
    m_chunkLoadCount = pu;
}

void WorldLoader::loadUnloadChunks()
{
    // NOT FINISHED
}
