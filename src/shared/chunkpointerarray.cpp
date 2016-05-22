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

#include "chunkpointerarray.h"

void ChunkPointerArray::move(const Vec3i& delta)
{
	Chunk** arr = new Chunk*[m_size3];
    Vec3i pos;
	for (pos.x = 0; pos.x < m_size; pos.x++) {
		for (pos.y = 0; pos.y < m_size; pos.y++) {
			for (pos.z = 0; pos.z < m_size; pos.z++) {
				if (elementExists(pos + delta))
                    arr[pos.x*m_size2 + pos.y*m_size + pos.z] = m_array[(pos.x + delta.x)*m_size2 + (pos.y + delta.y)*m_size + (pos.z + delta.z)];
				else arr[pos.x*m_size2 + pos.y*m_size + pos.z] = nullptr;
			}
		}
	}
	delete[] m_array;
	m_array = arr;
    m_org = m_org + delta;
}
