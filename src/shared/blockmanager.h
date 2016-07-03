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

#ifndef BLOCKMANAGER_H_
#define BLOCKMANAGER_H_

#include <vector>
using std::vector;

#include "blocktype.h"
#include "logger.h"

class BlockManager
{
private:
    vector<BlockType> blocks;

public:
    void registerBlock(const BlockType& block)
    {
        blocks.push_back(block);
        debugstream << "Registered block \"" << block.getName() << "\", attributes:\n"
                    << "Solid: " << block.isSolid() << "\n"
                    << "Translucent: " << block.isTranslucent() << "\n"
                    << "Opaque: " << block.isOpaque() << "\n"
                    << "Explode power: " << block.getExplodePower() << "\n"
                    << "Hardness: " << block.getHardness() << "\n";
    }

};

#endif // !BLOCKMANAGER_H_
