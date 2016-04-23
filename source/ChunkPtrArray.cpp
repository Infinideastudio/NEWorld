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

#include "chunkPtrArray.h"

namespace World
{

void chunkPtrArray::setSize(int s)
{
    size = s;
    size2 = size*size;
    size3 = size*size*size;
}

chunkPtrArray::chunkPtrArray()
{
    setSize((ViewDistance + 2) * 2);
    array = vector<chunk*>(size3, nullptr);
}

void chunkPtrArray::move(int xd, int yd, int zd)
{
    vector<chunk*> arrTemp(size3);
    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            for (int z = 0; z < size; z++)
            {
                if (elementExists(x + xd, y + yd, z + zd))
                    arrTemp[x*size2 + y*size + z] = array[(x + xd)*size2 + (y + yd)*size + (z + zd)];
                else
                    arrTemp[x*size2 + y*size + z] = nullptr;
            }
        }
    }
    array = arrTemp;
    originX += xd;
    originY += yd;
    originZ += zd;
}

void chunkPtrArray::moveTo(int x, int y, int z)
{
    move(x - originX, y - originY, z - originZ);
}

void chunkPtrArray::AddChunk(chunk* cptr, int cx, int cy, int cz)
{
    cx -= originX;
    cy -= originY;
    cz -= originZ;
    if (elementExists(cx, cy, cz))
        array[cx*size2 + cy*size + cz] = cptr;
}

void chunkPtrArray::DeleteChunk(int cx, int cy, int cz)
{
    cx -= originX;
    cy -= originY;
    cz -= originZ;
    if (elementExists(cx, cy, cz))
        array[cx*size2 + cy*size + cz] = nullptr;
}

chunk* chunkPtrArray::getChunkPtr(int x, int y, int z)
{
    x -= originX;
    y -= originY;
    z -= originZ;
    if (!elementExists(x, y, z))
        return nullptr;
    return array[x*size2 + y*size + z];
}

void chunkPtrArray::setChunkPtr(int x, int y, int z, const chunk* c)
{
    x -= originX;
    y -= originY;
    z -= originZ;
    assert(elementExists(x, y, z));
    array[x*size2 + y*size + z] = (chunk*)c;
}

}
