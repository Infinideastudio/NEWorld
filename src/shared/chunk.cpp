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

// 新代码
// 仅供示例，无法编译

#include "chunk.h"
using namespace std;

u64 Chunk::calc_hash()
{
    int x, y, z;
    u64 result = 0;

    // 将result与方块进行哈希
    // 此处采用BKDR哈希算法
    // 参考文献: The C Programming Language
    // -- Brian Kernighan & Dennis Ritchie
    for(x = 0; x < chunk_size; x++)
        for(y = 0; y < chunk_size; y++)
            for(z = 0; z < chunk_size; z++)
                // 在Intel x86平台上，乘法的时钟周期随着微架构的不同而不同
                // 较新的微架构上，乘法所花费的时钟周期可能已经被优化得比较小
                // 而在ARM平台上，乘法需要花费4个时钟周期或者更小
                // 所以将其分解为位运算可能并不经济
                // result = result << 7 + result << 1 + result + m_blocks[x][y][z].get_block_guid();
                result = result * 131 + m_blocks[x][y][z].get_block_guid();

    // 将result与地面掉落物进行哈希
    // ...

    return result;
}

