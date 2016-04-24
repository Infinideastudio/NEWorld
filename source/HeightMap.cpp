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

#include "HeightMap.h"
#include "WorldGen.h"
#include "World.h"

namespace World
{

void HeightMap::setSize(int s)
{
    size = s;
    size2 = size*size;
}

void HeightMap::create()
{
    array = new int[size2];
    memset(array, -1, size2*sizeof(int));
}

void HeightMap::destroy()
{
    delete[] array;
    array = 0;
}

void HeightMap::move(int xd, int zd)
{
    int* arrTemp = new int[size2];
    for (int x = 0; x < size; x++)
    {
        for (int z = 0; z < size; z++)
        {
            if (x + xd >= 0 && z + zd >= 0 && x + xd<size && z + zd<size)
                arrTemp[x*size + z] = array[(x + xd)*size + (z + zd)];
            else arrTemp[x*size + z] = -1;
        }
    }
    delete[] array;
    array = arrTemp;
    originX += xd;
    originZ += zd;
}

void HeightMap::moveTo(int x, int z)
{
    move(x - originX, z - originZ);
}

int HeightMap::getHeight(int x, int z)
{
    x -= originX;
    z -= originZ;
    if (x < 0 || z < 0 || x >= size || z >= size)
        return pWorldGen->getHeight(x + originX, z + originZ);
    if (array[x*size + z] == -1)
        array[x*size + z] = pWorldGen->getHeight(x + originX, z + originZ);
    return array[x*size + z];
}
}
