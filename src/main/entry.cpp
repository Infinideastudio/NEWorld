// 
// MainPlugin: entry.cpp
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

#include "worldgen.h"
#include "api/nwapigui.hpp"
#include "Common/EventBus.h"
#include "Common/Modules.h"

int32_t GrassID = 0, RockID, DirtID, SandID, WaterID;

int32_t registerBlock(const char* name, bool solid, bool translucent, bool opaque,
                      int explodepower, int hardness) {
    NWblocktype block;
    block.blockname = name;
    block.solid = solid;
    block.translucent = translucent;
    block.opaque = opaque;
    block.explodePower = explodepower;
    block.hardness = hardness;
    return nwRegisterBlock(&block);
}

class MainModule : public ModuleObject {
public:
    MainModule() {
        nwRegisterChunkGenerator(generator);
        GrassID = registerBlock("Grass", true, false, true, 0, 2);
        RockID = registerBlock("Rock", true, false, true, 0, 2);
        DirtID = registerBlock("Dirt", true, false, true, 0, 2);
        SandID = registerBlock("Sand", true, false, true, 0, 2);
        WaterID = registerBlock("Water", false, true, false, 0, 2);
        try {
            rendererInit();
        }
        catch(...){}
    }

    void rendererInit() {
        NWtextureid id[] =
        {
            CALL_AUTO(nwRegisterTexture, "./res/blocks/grass_top.png"),
            CALL_AUTO(nwRegisterTexture, "./res/blocks/grass_round.png"),
            CALL_AUTO(nwRegisterTexture, "./res/blocks/dirt.png"),
            CALL_AUTO(nwRegisterTexture, "./res/blocks/rock.png"),
            CALL_AUTO(nwRegisterTexture, "./res/blocks/sand.png"),
            CALL_AUTO(nwRegisterTexture, "./res/blocks/water.png")
        };
        NWblocktexture grass{ id[1], id[1], id[0], id[2], id[1], id[1] };
        NWblocktexture rock{ id[3], id[3], id[3], id[3], id[3], id[3] };
        NWblocktexture dirt{ id[2], id[2], id[2], id[2], id[2], id[2] };
        NWblocktexture sand{ id[4], id[4], id[4], id[4], id[4], id[4] };
        NWblocktexture water{ id[5], id[5], id[5], id[5], id[5], id[5] };
        CALL_AUTO(nwUseDefaultBlockRenderFunc, GrassID, &grass);
        CALL_AUTO(nwUseDefaultBlockRenderFunc, RockID, &rock);
        CALL_AUTO(nwUseDefaultBlockRenderFunc, DirtID, &dirt);
        CALL_AUTO(nwUseDefaultBlockRenderFunc, SandID, &sand);
        CALL_AUTO(nwUseDefaultBlockRenderFunc, WaterID, &water);
    }
};

extern "C" {

    NWAPIEXPORT const char* NWAPICALL nwModuleGetInfo() {
        return
            R"(
{
    "name" : "Main",
    "author" : "INFINIDEAS",
    "uri" : "infinideas.neworld.main",
    "version" : [0, 0, 1, 0],
    "dependencies" : [
        { "uri" : "infinideas.neworld.gui", "required" : [0, 0, 1, 0], "optional" : true }
    ]
}
)";
    }

    // Main function
    NWAPIEXPORT ModuleObject* NWAPICALL nwModuleGetObject() {
        return new MainModule();
    }

}
