#ifndef BLOCKS_H
#define BLOCKS_H

#include "Definitions.h"
#include "Globalization.h"

void* Allocator(string key);

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
    SingleBlock(string blockName, bool solid, bool opaque, bool translucent, bool _explosive, float _hardness) :
        name(blockName), Solid(solid), Opaque(opaque), Translucent(translucent), explosive(_explosive), Hardness(_hardness) {};

    virtual bool ExecBUF(BUDDP * args);

    virtual string getBlockName();

    virtual bool isSolid();

    virtual bool isOpaque();

    virtual bool isTranslucent();

    virtual bool isExplosive();

    virtual float getHardness();
};

extern SingleBlock* blockData[BLOCK_DEF_END + 1];

}

inline Blocks::SingleBlock BlockInfo(block blockID)
{
    return (*Blocks::blockData[(blockID).ID >= Blocks::BLOCK_DEF_END || (blockID.ID) < 0 ? Blocks::BLOCK_DEF_END : (blockID.ID)]);
}
#endif
