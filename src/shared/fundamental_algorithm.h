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

#ifndef FUNDAMENTAL_ALGORITHM
#define FUNDAMENTAL_ALGORITHM

#include <string>
#include "fundamental_structure.h"

// BKDR哈希算法 默认的常量因子为131 可以手动调整
u64 bkdr_hash(const std::string &str, u64 magic_constant = 131);

#endif
