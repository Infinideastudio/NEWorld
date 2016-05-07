#include "blocktype.h"

std::string BlockType::getName() const
{
    return name;
}

bool BlockType::isSolid() const
{
    return solid;
}

bool BlockType::isTranslucent() const
{
    return translucent;
}

bool BlockType::isOpaque() const
{
    return opaque;
}

int BlockType::getExplodePower() const
{ 
    return explodePower;
}

int BlockType::getHardness() const
{
    return hardness;
}
