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

#ifndef CHUNKPACKET_H
#define CHUNKPACKET_H

#include "Definitions.h"
enum PacketHeaders { CHUNK_DATA = 2, CHUNK_COMMAND, CHUNK_EMPTY, CHUNK_NOTBEMODIFIED };
struct ChunkPacket
{
    int cx, cy, cz;
    block pblocks[4096];
    brightness pbrightness[4096];
};

enum ChunkCommands { CHUNK_COMMAND_CHANGEBLOCK };
struct ChunkCommand
{
    int command;
    int cx, cy, cz;
    int x, y, z;
    int extraInfo;
};

#endif
