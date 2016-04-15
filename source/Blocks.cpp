#include "Blocks.h"

namespace Blocks
{
    SingleBlock* blockData[BLOCK_DEF_END + 1] =
    {
        (SingleBlock*)Allocator("NEWorld.Blocks.Air"),
        (SingleBlock*)Allocator("NEWorld.Blocks.Rock"),
        (SingleBlock*)Allocator("NEWorld.Blocks.Grass"),
        (SingleBlock*)Allocator("NEWorld.Blocks.Dirt"),
        (SingleBlock*)Allocator("NEWorld.Blocks.Stone"),
        (SingleBlock*)Allocator("NEWorld.Blocks.Plank"),
        (SingleBlock*)Allocator("NEWorld.Blocks.Wood"),
        (SingleBlock*)Allocator("NEWorld.Blocks.Bedrock"),
        (SingleBlock*)Allocator("NEWorld.Blocks.Leaf"),
        (SingleBlock*)Allocator("NEWorld.Blocks.Glass"),
        (SingleBlock*)Allocator("NEWorld.Blocks.Water"),
        (SingleBlock*)Allocator("NEWorld.Blocks.Lava"),
        (SingleBlock*)Allocator("NEWorld.Blocks.GlowStone"),
        (SingleBlock*)Allocator("NEWorld.Blocks.Sand"),
        (SingleBlock*)Allocator("NEWorld.Blocks.Cement"),
        (SingleBlock*)Allocator("NEWorld.Blocks.Ice"),
        (SingleBlock*)Allocator("NEWorld.Blocks.Coal Block"),
        (SingleBlock*)Allocator("NEWorld.Blocks.Iron Block"),
        (SingleBlock*)Allocator("NEWorld.Blocks.TNT"),
        (SingleBlock*)Allocator("NEWorld.Blocks.Null Block")
    };

    bool SingleBlock::ExecBUF(BUDDP * args)
    {
        return false;
    }

    string SingleBlock::getBlockName()
    {
        return Globalization::GetStr(name);
    }

    bool SingleBlock::isSolid()
    {
        return Solid;
    }

    bool SingleBlock::isOpaque()
    {
        return Opaque;
    }

    bool SingleBlock::isTranslucent()
    {
        return Translucent;
    }

    bool SingleBlock::isExplosive()
    {
        return explosive;
    }

    float SingleBlock::getHardness()
    {
        return Hardness;
    }

}