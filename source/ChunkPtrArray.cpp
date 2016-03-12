#include "chunkPtrArray.h"

namespace World
{

void chunkPtrArray::setSize(int s)
{
    size = s;
    size2 = size*size;
    size3 = size*size*size;
}

bool chunkPtrArray::create()
{
    array = new chunk*[size3];
    if (array == nullptr) return false;
    memset(array, (int)nullptr, size3*sizeof(chunk*));
    return true;
}

void chunkPtrArray::destroy()
{
    delete[] array;
    array = nullptr;
}

void chunkPtrArray::move(int xd, int yd, int zd)
{
    chunk** arrTemp = new chunk*[size3];
    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            for (int z = 0; z < size; z++)
            {
                if (elementExists(x + xd, y + yd, z + zd))
                    arrTemp[x*size2 + y*size + z] = array[(x + xd)*size2 + (y + yd)*size + (z + zd)];
                else
                    arrTemp[x*size2 + y*size + z] = nullptr;
            }
        }
    }
    delete[] array;
    array = arrTemp;
    originX += xd;
    originY += yd;
    originZ += zd;
}

void chunkPtrArray::moveTo(int x, int y, int z)
{
    move(x - originX, y - originY, z - originZ);
}

void chunkPtrArray::AddChunk(chunk* cptr, int cx, int cy, int cz)
{
    cx -= originX;
    cy -= originY;
    cz -= originZ;
    if (elementExists(cx, cy, cz))
        array[cx*size2 + cy*size + cz] = cptr;
}

void chunkPtrArray::DeleteChunk(int cx, int cy, int cz)
{
    cx -= originX;
    cy -= originY;
    cz -= originZ;
    if (elementExists(cx, cy, cz))
        array[cx*size2 + cy*size + cz] = nullptr;
}

chunk* chunkPtrArray::getChunkPtr(int x, int y, int z)
{
    x -= originX;
    y -= originY;
    z -= originZ;
    if (!elementExists(x, y, z))
        return nullptr;
#ifdef NEWORLD_DEBUG_PERFORMANCE_REC
    c_getChunkPtrFromCPA++;
#endif
    return array[x*size2 + y*size + z];
}

void chunkPtrArray::setChunkPtr(int x, int y, int z, chunk* c)
{
    x -= originX;
    y -= originY;
    z -= originZ;
    if (!elementExists(x, y, z))
        return;
    array[x*size2 + y*size + z] = c;
}

}
