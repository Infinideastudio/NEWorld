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

#include "chunkloader.h"

void ChunkLoader::build() const
{
    // TODO: Try to use terrain generators written in plugins
    // This is the default terrain generator. Use this when no generators were loaded from plugins.
    for (int x = 0; x < ChunkSize; x++)
        for (int z = 0; z < ChunkSize; z++)
            for (int y = 0; y < ChunkSize; y++)
            {
                // TODO: Set block to BlockData(0, DaylightBrightness, 0)
                m_chunk->setBlock(Vec3i(x, y, z), BlockData(0, 0, 0)); // NOT FINISHED
            }
}
