#ifndef BLOCKS_H
#define BLOCKS_H
#include "stdinclude.h"
#include "Typedefs.h"
#include "Definitions.h"
#include "Globalization.h"
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

    //获得方块名称
    string getBlockName() const
    {
        return Globalization::GetStrbyKey(name);
    }

    //是否是固体
    bool isSolid() const
    {
        return Solid;
    }

    //是否不透明
    bool isOpaque() const
    {
        return Opaque;
    }

    //是否半透明
    bool isTranslucent() const
    {
        return Translucent;
    }

    //是否可以爆炸
    bool isExplosive() const
    {
        return explosive;
    }

    //获得硬度（数值越大硬度越小，最大100）
    float getHardness() const
    {
        return Hardness;
    }
};

const SingleBlock blockData[BLOCK_DEF_END + 1] =
{
    //			方块名称		  固体	 不透明	  半透明  可以爆炸  硬度 方块更新回调函数
    SingleBlock("NEWorld.Blocks.Air"		, false	, false	, false , false,	0    , nullptr),
    SingleBlock("NEWorld.Blocks.Rock"		, true	, true	, false , false,	2    , nullptr),
    SingleBlock("NEWorld.Blocks.Grass"		, true	, true	, false , false,	5    , &GrassBUF),
    SingleBlock("NEWorld.Blocks.Dirt"		, true	, true	, false , false,	5    , nullptr),
    SingleBlock("NEWorld.Blocks.Stone"		, true	, true	, false , false,	2    , nullptr),
    SingleBlock("NEWorld.Blocks.Plank"		, true	, true	, false , false,	5    , nullptr),
    SingleBlock("NEWorld.Blocks.Wood"		, true	, true	, false , false,	5    , nullptr),
    SingleBlock("NEWorld.Blocks.Bedrock"	, true	, true	, false , false,	0    , nullptr),
    SingleBlock("NEWorld.Blocks.Leaf"		, true	, false	, false	, false,	15   , nullptr),
    SingleBlock("NEWorld.Blocks.Glass"		, true	, false	, false	, false,	30   , nullptr),
    SingleBlock("NEWorld.Blocks.Water"		, false	, false	, true	, false,	0    , &WaterBUF),
    SingleBlock("NEWorld.Blocks.Lava"		, false	, false	, true	, false,	0    , nullptr),
    SingleBlock("NEWorld.Blocks.GlowStone"	, true	, true	, false	, false,	10   , nullptr),
    SingleBlock("NEWorld.Blocks.Sand"		, true	, true	, false	, false,	8    , nullptr),
    SingleBlock("NEWorld.Blocks.Cement"		, true	, true	, false	, false,	0.5f , nullptr),
    SingleBlock("NEWorld.Blocks.Ice"		, true	, false	, true  , false,	25   , nullptr),
    SingleBlock("NEWorld.Blocks.Coal Block" , true	, true	, false , false,	1    , nullptr),
    SingleBlock("NEWorld.Blocks.Iron Block" , true	, true	, false , false,	0.5f , nullptr),
    SingleBlock("NEWorld.Blocks.TNT"		, true	, true	, false , true,		25   , nullptr),
    SingleBlock("NEWorld.Blocks.Null Block" , true  , true  , false , false,	0    , nullptr)
};

}

#define BlockInfo(blockID) Blocks::blockData[(blockID).ID >= Blocks::BLOCK_DEF_END || (blockID.ID) < 0 ? Blocks::BLOCK_DEF_END : (blockID.ID)]
#endif
