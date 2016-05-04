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

#ifndef CHUNKPTRARRAY_H
#define CHUNKPTRARRAY_H

#include "Definitions.h"

namespace World
{

    class chunk;
    struct chunkPtrArray
    {
        vector<chunk*> array;
        int originX, originY, originZ, size, size2, size3;
        void setSize(int s);
        chunkPtrArray();
        void move(int xd, int yd, int zd);
        void moveTo(int x, int y, int z);
        void AddChunk(chunk* cptr, int cx, int cy, int cz);
        void DeleteChunk(int cx, int cy, int cz);
        bool elementExists(int x, int y, int z)
        {
            return x >= 0 && x < size && z >= 0 && z < size && y >= 0 && y < size;
        }
        chunk* getChunkPtr(int x, int y, int z);
        void setChunkPtr(int x, int y, int z, const chunk* c);
    };

}

#endif
