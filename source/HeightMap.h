#pragma once
#include "Definitions.h"

namespace World
{
chunkid getChunkID(int x, int y, int z);
struct HeightMap
{
    int* array = nullptr;
    int size, size2;
    int originX, originZ;
    void setSize(int s);
    void create();
    void destroy();
    void move(int xd, int zd);
    void moveTo(int x, int z);
    int getHeight(int x, int z);
};
}
