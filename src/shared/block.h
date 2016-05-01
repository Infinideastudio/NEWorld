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

// 一种方块
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
};

#ifdef BLOCKTYPE2
class BlockType2
{
    private:
        std::string symbol;
        u16 ID; //This value is dynamic!!
    public:
        virtual std::string get_translation(Block& b); //函数体略 缺省翻译键为symbol
        virtual void norm_update(Block& b, Block& ori);//normal bupdate function
        virtual void tick_update(Block& b);
        virtual AABB get_hitbox(Block& b);
        //以下省略一些获取属性的函数
}

//为了避免一些evil的方块ID问题，同时尽可能减小内存使用，ID做如下调整
std::vector<BlockType2*> BlockTypes;//储存BT的序列，为提升性能可能会牺牲少量内存（空ID）
std::set<u16> occupied_ids; // 储存了被占用的ID
//BT管理函数
void add_block(std::string symbol);// TODO:将第一个空ID分配给标示为Symbol的方块
void del_block(u16 ID);//TODO:移除ID上的BlockType
//加载
void load_blocktypes();//TODO:先从世界存档的config里读取每个ID对应的方块，然后通过Mod以及游戏主程序中的void* getobject(std::string symbol)函数获取BlockType
void finalize();//TODO:释放并保存
#endif

// 一个方块实例
class Block : public BlockType
{
};

#endif
