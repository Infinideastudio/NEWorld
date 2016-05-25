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

#ifndef CHUNKPOINTERARRAY_H_
#define CHUNKPOINTERARRAY_H_

#include <cstring> // memset
#include "chunk.h"

class ChunkPointerArray
{
private:
    // Array
    Chunk** m_array;
    // Array size
    int m_size, m_size2, m_size3;
    // Origin
    Vec3i m_org;

public:
    ChunkPointerArray(int size)
    {
        m_size = size;
        m_size2 = size*size;
        m_size3 = size*size*size;
        m_array = new Chunk*[m_size3];
        memset(m_array, 0, m_size3*sizeof(Chunk*));
    }
    ~ChunkPointerArray()
    {
        delete[] m_array;
    }

    // Move array by delta
    void move(const Vec3i& delta);
    // Move array to pos
    void moveTo(const Vec3i& pos)
    {
        move(pos - m_org);
    }
    // Check if specific element is inside array range
    bool elementExists(const Vec3i& pos) const
    {
        return pos.x >= 0 && pos.x < m_size && pos.z >= 0 && pos.z < m_size && pos.y >= 0 && pos.y < m_size;
    }
    // Get chunk pointer from array
    Chunk* getChunkPtr(Vec3i pos) const
    {
        pos = pos - m_org;
        if (!elementExists(pos)) return nullptr;
        return m_array[pos.x*m_size2 + pos.y*m_size + pos.z];
    }
    // Update chunk pointer in array
    void setChunkPtr(Vec3i pos, Chunk* c)
    {
        pos = pos - m_org;
        if (!elementExists(pos)) return;
        m_array[pos.x*m_size2 + pos.y*m_size + pos.z] = c;
    }

};

#endif
