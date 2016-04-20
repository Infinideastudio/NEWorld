#ifndef CHUNKPTRARRAY_H
#define CHUNKPTRARRAY_H

#include "Definitions.h"

namespace World
{

class chunk;
struct chunkPtrArray
{
    vector<chunk*> array;
    int originX, originY, originZ, size, size2, size3;
    void setSize(int s);
    chunkPtrArray();
    void move(int xd, int yd, int zd);
    void moveTo(int x, int y, int z);
    void AddChunk(chunk* cptr, int cx, int cy, int cz);
    void DeleteChunk(int cx, int cy, int cz);
    bool elementExists(int x, int y, int z)
    {
        return x >= 0 && x < size && z >= 0 && z < size && y >= 0 && y < size;
    }
    chunk* getChunkPtr(int x, int y, int z);
    void setChunkPtr(int x, int y, int z, const chunk* c);
};

}

#endif
