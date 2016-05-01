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

#include "fundamental_algorithm.h"
using namespace std;

u64 bkdr_hash(const string &str, u64 magic_constant = 131)
{
    // 此处采用BKDR哈希算法
    // 参考文献: The C Programming Language
    // -- Brian Kernighan & Dennis Ritchie
    u64 result = 0;
    for(int i = 0, size = str.size(); i < size; i++)
        result = result * magic_constant + str[i];
    return result;
}

