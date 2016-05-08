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

#include "worldloader.h"

void WorldLoader::sortChunkLoadUnloadList(const Vec3& centerPos)
{
    Vec3 centerCPos, cur, distvec;
    Chunk* cur;
    int pl = 0, pu = 0;
    int distsqr, first, middle, last;

    // centerPos to chunk coords
    centerCPos = _world->getChunkPos(centerPos);

    for (size_t ci = 0; ci < _world->getChunkCount(); ci++)
    {
        cur = _world->getChunkPtr(ci)->getPos();
        // Get chunk center pos
        cur.x = (cur.x << ChunkSizeLog2) + ((ChunkSize >> 1) - 1);
        cur.y = (cur.y << ChunkSizeLog2) + ((ChunkSize >> 1) - 1);
        cur.z = (cur.z << ChunkSizeLog2) + ((ChunkSize >> 1) - 1);

        // Out of load range, pending to unload
        if (/*!chunkInRange(cx, cy, cz, cxp, cyp, czp, ViewDistance + 1)*/ true /* NOT FINISHED YET */)
        {
            // Distance from centerPos
            distvec = cur - centerPos;
            distsqr = distvec.lengthSqr();

            // Binary search in unload list
            first = 0;
            last = pl - 1;
            while (first <= last)
            {
                middle = (first + last) >> 1;
                if (distsqr > _chunkUnloadList[middle].second)
                    last = middle - 1;
                else
                    first = middle + 1;
            }

            // Not very far, don't unload now
            if (first > pl || first >= MaxChunkUnloadCount) continue;

            // Move elements to make place
            for (int j = MaxChunkUnloadCount - 1; j > first; j--)
            {
                _chunkUnloadList[j].first = _chunkUnloadList[j - 1].first;
                _chunkUnloadList[j].second = _chunkUnloadList[j - 1].second;
            }

            // Insert into list
            _chunkUnloadList[first].first = _world->getChunkPtr(ci);
            _chunkUnloadList[first].second = distsqr;

            // Add counter
            if (pl < MaxChunkUnloadCount) pl++;
        }
    }
    _chunkUnloadCount = pl;

    // NOT FINISHED YET

    /*
    for (cx = cxp - ViewDistance - 1; cx <= cxp + ViewDistance; cx++)
    {
        for (cy = cyp - ViewDistance - 1; cy <= cyp + ViewDistance; cy++)
        {
            for (cz = czp - ViewDistance - 1; cz <= czp + ViewDistance; cz++)
            {
                if (chunkOutOfBound(cx, cy, cz)) continue;
                if (cpArray.getChunkPtr(cx, cy, cz) == nullptr)
                {
                    xd = cx * 16 + 7 - xpos;
                    yd = cy * 16 + 7 - ypos;
                    zd = cz * 16 + 7 - zpos;
                    distsqr = xd*xd + yd*yd + zd*zd;

                    first = 0;
                    last = pu - 1;
                    while (first <= last)
                    {
                        middle = (first + last) >> 1;
                        if (distsqr < chunkLoadList[middle][0])
                            last = middle - 1;
                        else
                            first = middle + 1;
                    }
                    if (first > pu || first >= MaxChunkLoads) continue;
                    i = first;

                    for (int j = MaxChunkLoads - 1; j > i; j--)
                    {
                        chunkLoadList[j][0] = chunkLoadList[j - 1][0];
                        chunkLoadList[j][1] = chunkLoadList[j - 1][1];
                        chunkLoadList[j][2] = chunkLoadList[j - 1][2];
                        chunkLoadList[j][3] = chunkLoadList[j - 1][3];
                    }
                    chunkLoadList[i][0] = distsqr;
                    chunkLoadList[i][1] = cx;
                    chunkLoadList[i][2] = cy;
                    chunkLoadList[i][3] = cz;
                    if (pu < MaxChunkLoads) pu++;
                }
            }
        }
    }
    chunkLoads = pu;
    */
}

void WorldLoader::loadUnloadChunks()
{
    // NOT FINISHED YET
}
