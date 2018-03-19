// 
// Core: nwserverapi.cpp
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

#include "Game/Context/nwcontext.hpp"
#include "Game/SyncService/world/nwblock.h"
#include "Game/Api/ApiCore.h"
#include "Game/SyncService/world/nwchunk.h"

namespace PluginAPI {

    BlockData convertBlockData(const NWblockdata& src) { return BlockData(src.id, src.brightness, src.state); }

    NWblockdata convertBlockData(const BlockData& src) {
        NWblockdata res;
        res.id = src.getID();
        res.brightness = src.getBrightness();
        res.state = src.getState();
        return res;
    }

    BlockType convertBlockType(const NWblocktype& src) {
        return BlockType(src.blockname, src.solid, src.translucent, src.opaque, src.explodePower, src.hardness);
    }
}

extern "C" {
    using namespace PluginAPI;

    NWAPI size_t NWAPICALL nwRegisterBlock(const NWblocktype* block) {
        return context.blocks.registerBlock(convertBlockType(*block));
    }

    NWAPI void NWAPICALL nwLog(char* str) { }

    NWAPI size_t NWAPICALL nwRegisterChunkGenerator(NWchunkgenerator* const generator) {
        if (Chunk::ChunkGeneratorLoaded) {
            warningstream << "Ignoring multiple chunk generators!";
            return 1;
        }
        Chunk::ChunkGeneratorLoaded = true;
        Chunk::ChunkGen = reinterpret_cast<ChunkGenerator*>(generator);
        debugstream << "Registered chunk generator";
        return 0;
    }
}
