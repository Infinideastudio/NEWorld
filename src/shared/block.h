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

#ifndef BLOCK_H
#define BLOCH_H

#include <string>
#include "fundamental_structure.h"
#include "fundamental_algorithm.h"

// 方块类型
class BlockType
{
    private:
        // 内部方块名称
        std::string m_name;

        // 翻译后的名称
        std::string m_name_translated;

        // 为了避免Minecraft中evil的ID重号问题，在NEWorld中，
        // 统一使用由内部方块名称哈希计算得到的GUID作为标识符。
        u64 m_guid;

    public:
        // 获取GUID
        u64 get_guid()
        {
            return m_guid;
        }

        // 获取这个方块类型的内部符号名称
        virtual std::string get_symbol();
        
        // 获取翻译后准备显示在屏幕上的名称
        virtual std::string get_translated_name();
        
        virtual void norm_update(Block& b, Block& ori);

        virtual void tick_update(Block& b);
        
        virtual AABB get_hitbox();
};

std::map<std::string, BlockType> BlockTypesTable;

// 初始化方块类型表
void init_block_types_table();

// 方块实例
class Block : public BlockType
{
};

class BlockOven : public Block
{
    private:
        // 被烤的物品
        // 燃料物品
        // 成品物品
        // 烘烤的状态
}

#endif
