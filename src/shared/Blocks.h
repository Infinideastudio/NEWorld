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

#ifndef BLOCKS_H
#define BLOCKS_H

#include "Definitions.h"
#include "BlockFuncs.h"
namespace Blocks
{

enum MineType
{
    Solid, Liquid, Gas, Sand, Plasma, Special, Minetype_End
};

enum BlockID
{
    AIR, ROCK, GRASS, DIRT, STONE, PLANK, WOOD, BEDROCK, LEAF,
    GLASS, WATER, LAVA, GLOWSTONE, SAND, CEMENT, ICE, COAL, IRON,
    TNT, BLOCK_DEF_END
};

const block NONEMPTY = block(1);

typedef bool(*BUDF)(BUDDP* arg);
typedef bool(*TILF)(TILDP* arg);
class SingleBlock
{
private:
    string name;
    float Hardness;
    bool Solid;
    bool Opaque;
    bool Translucent;
    bool Dark;
    bool explosive;
    int MineType;

public:

    BUDF BlockUpdateFunc = nullptr;
    TILF BlockTickUpdateFunc = nullptr;

    bool ExecBUF(BUDDP b) const
    {
        bool ret = false;
        if (BlockUpdateFunc)
        {
            ret = BlockUpdateFunc(&b);
        }
        return ret;
    }

    SingleBlock(string blockName, bool solid, bool opaque, bool translucent, bool _explosive, float _hardness, BUDF _buf) :
        name(blockName), Solid(solid), Opaque(opaque), Translucent(translucent), explosive(_explosive), Hardness(_hardness), BlockUpdateFunc(_buf) {};

    //string getBlockName() const;
    //{
    //    return Globalization::GetStrbyKey(name);
    //}

    bool isSolid() const
    {
        return Solid;
    }

    bool isOpaque() const
    {
        return Opaque;
    }

    bool isTranslucent() const
    {
        return Translucent;
    }

    bool isExplosive() const
    {
        return explosive;
    }

    float getHardness() const
    {
        return Hardness;
    }
};

const SingleBlock blockData[BLOCK_DEF_END + 1] =
{
    SingleBlock("NEWorld.Blocks.Air"        , false    , false    , false , false,    0    , nullptr),
    SingleBlock("NEWorld.Blocks.Rock"        , true    , true    , false , false,    2    , nullptr),
    SingleBlock("NEWorld.Blocks.Grass"        , true    , true    , false , false,    5    , &GrassBUF),
    SingleBlock("NEWorld.Blocks.Dirt"        , true    , true    , false , false,    5    , nullptr),
    SingleBlock("NEWorld.Blocks.Stone"        , true    , true    , false , false,    2    , nullptr),
    SingleBlock("NEWorld.Blocks.Plank"        , true    , true    , false , false,    5    , nullptr),
    SingleBlock("NEWorld.Blocks.Wood"        , true    , true    , false , false,    5    , nullptr),
    SingleBlock("NEWorld.Blocks.Bedrock"    , true    , true    , false , false,    0    , nullptr),
    SingleBlock("NEWorld.Blocks.Leaf"        , true    , false    , false    , false,    15   , nullptr),
    SingleBlock("NEWorld.Blocks.Glass"        , true    , false    , false    , false,    30   , nullptr),
    SingleBlock("NEWorld.Blocks.Water"        , false    , false    , true    , false,    0    , &WaterBUF),
    SingleBlock("NEWorld.Blocks.Lava"        , false    , false    , true    , false,    0    , &LavaBUF),
    SingleBlock("NEWorld.Blocks.GlowStone"    , true    , true    , false    , false,    10   , nullptr),
    SingleBlock("NEWorld.Blocks.Sand"        , true    , true    , false    , false,    8    , nullptr),
    SingleBlock("NEWorld.Blocks.Cement"        , true    , true    , false    , false,    0.5f , nullptr),
    SingleBlock("NEWorld.Blocks.Ice"        , true    , false    , true  , false,    25   , nullptr),
    SingleBlock("NEWorld.Blocks.Coal Block" , true    , true    , false , false,    1    , nullptr),
    SingleBlock("NEWorld.Blocks.Iron Block" , true    , true    , false , false,    0.5f , nullptr),
    SingleBlock("NEWorld.Blocks.TNT"        , true    , true    , false , true,        25   , nullptr),
    SingleBlock("NEWorld.Blocks.Null Block" , true  , true  , false , false,    0    , nullptr)
};

}

#define BlockInfo(blockID) Blocks::blockData[(blockID).ID >= Blocks::BLOCK_DEF_END || (blockID.ID) < 0 ? Blocks::BLOCK_DEF_END : (blockID.ID)]

#endif
