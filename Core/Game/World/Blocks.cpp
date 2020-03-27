// 
// Core: nwblock.cpp
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#include "Blocks.h"
#include "Common/Logger.h"

namespace Game::World {
    Blocks::Blocks()
            :mBlocks{BlockType("Air", false, false, false, 0)} { }

    size_t Blocks::registerBlock(const BlockType& block) {
        mBlocks.push_back(block);
        auto id = mBlocks.size()-1;
        Game::World::BlockType type = mBlocks[id];
        debugstream << "Registered block:"
                    << "Block \"" << type.getName() << "\"(ID = " << id << ") = {"
                    << "Solid: " << type.isSolid()
                    << ", Translucent: " << type.isTranslucent()
                    << ", Opaque: " << type.isOpaque()
                    << ", Hardness: " << type.getHardness()
                    << "}";
        return id;
    }

    Blocks& Blocks::getInstance() {
        static Blocks mgr;
        return mgr;
    }
}