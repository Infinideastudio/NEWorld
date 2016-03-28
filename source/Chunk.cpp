#include "Chunk.h"
#include "WorldGen.h"
#include "World.h"
#include "Blocks.h"

namespace ChunkRenderer
{
void RenderChunk(World::chunk* c);
void MergeFaceRender(World::chunk* c);
void RenderDepthModel(World::chunk* c);
}

namespace Renderer
{
extern bool AdvancedRender;
}

namespace World
{

struct HMapManager
{
    int H[16][16];
    int low, high, count;
    HMapManager() {};
    HMapManager(int cx, int cz)
    {
        int l = MAXINT, hi = WorldGen::WaterLevel, h;
        for (int x = 0; x < 16; ++x)
        {
            for (int z = 0; z < 16; ++z)
            {
                h = HMap.getHeight(cx * 16 + x, cz * 16 + z);
                if (h < l) l = h;
                if (h > hi) hi = h;
                H[x][z] = h;
            }
        }
        low = (l - 21) / 16, high = (hi + 16) / 16;
        count = 0;
    }
};

inline string v22string(int x, int y)
{
    char * _ = (char*)malloc(sizeof(int) * 2+1);
    int * __ = (int*)_;
    __[0] = x;
    __[1] = y;
    _[sizeof(int) * 2] = '\0';
    string s = string(_);
    free(_);
    free(__);
    return string(_);
}

double chunk::relBaseX, chunk::relBaseY, chunk::relBaseZ;
Frustum chunk::TestFrustum;

void chunk::create()
{
    aabb = getBaseAABB();
    pblocks = new block[4096];
    pbrightness = new brightness[4096];
    //Note: If the memory is not enough, let it crash. Or you will have to face other 'strange' errors.
}

void chunk::destroy()
{
    delete[] pblocks;
    delete[] pbrightness;
    pblocks = nullptr;
    pbrightness = nullptr;
    updated = false;
    unloadedChunks++;
}

void chunk::buildTerrain(bool initIfEmpty)
{

    //Fast generate parts
    //Part1 out of the terrain bound
    if (cy > 4)
    {
        Empty = true;
        if (!initIfEmpty)
            return;
        memset(pblocks, 0, 4096 * sizeof(block));
        for (int i = 0; i < 4096; i++)
            pbrightness[i] = skylight;
        return;
    }
    if (cy < 0)
    {
        Empty = true;
        if (!initIfEmpty)
            return;
        memset(pblocks, 0, 4096 * sizeof(block));
        for (int i = 0; i < 4096; i++)
            pbrightness[i] = BRIGHTNESSMIN;
        return;
    }

    //Part2 out of geomentry area
    HMapManager cur = HMapManager(cx, cz);
    if (cy > cur.high)
    {
        Empty = true;
        if (!initIfEmpty)
            return;
        memset(pblocks, 0, 4096 * sizeof(block));
        for (int i = 0; i < 4096; i++)
            pbrightness[i] = skylight;
        return;
    }
    if (cy < cur.low)
    {
        for (int i = 0; i < 4096; i++)
            pblocks[i] = block(Blocks::ROCK);
        memset(pbrightness, 0, 4096 * sizeof(brightness));
        if (cy == 0)
            for (int x = 0; x < 16; x++)
                for (int z = 0; z < 16; z++)
                    pblocks[x * 256 + z] = block(Blocks::BEDROCK);
        Empty = false;
        return;
    }

    //Normal Calc
    //Init
    memset(pblocks, 0, 4096 * sizeof(block)); //Empty the chunk
    memset(pbrightness, 0, 4096 * sizeof(brightness)); //Set All Brightness to 0

    int x, z, h = 0, sh = 0, wh = 0, base, y;
    int minh, maxh, cur_br;

    Empty = true;
    sh = WorldGen::WaterLevel + 2 - (cy << 4);
    wh = WorldGen::WaterLevel - (cy << 4);

    for (x = 0; x < 16; ++x)
    {
        for (z = 0; z < 16; ++z)
        {
            base = (x << 8) + z;
            h = cur.H[x][z] - (cy << 4);
            if (h >= 0 || wh >= 0)
                Empty = false;
            if (h > sh && h > wh + 1)
            {
                //Grass layer
                if (h >= 0 && h < 16)
                    pblocks[(h << 4) + base] = block(Blocks::GRASS);
                //Dirt layer
                maxh = min(max(0, h), 16);
                for (y = min(max(0, h - 5), 16); y < maxh; ++y)
                    pblocks[(y << 4) + base] = block(Blocks::DIRT);
            }
            else
            {
                //Sand layer
                maxh = min(max(0, h + 1), 16);
                for (y = min(max(0, h - 5), 16); y < maxh; ++y)
                    pblocks[(y << 4) + base] = block(Blocks::SAND);
                //Water layer
                minh = min(max(0, h + 1), 16);
                maxh = min(max(0, wh + 1), 16);
                cur_br = BRIGHTNESSMAX - (WorldGen::WaterLevel - (maxh - 1 + (cy << 4))) * 2;
                if (cur_br < BRIGHTNESSMIN) cur_br = BRIGHTNESSMIN;
                for (y = maxh - 1; y >= minh; --y)
                {
                    pblocks[(y << 4) + base] = block(Blocks::WATER, 1, 255);
                    pbrightness[(y << 4) + base] = (brightness)cur_br;
                    cur_br -= 2;
                    if (cur_br < BRIGHTNESSMIN)
                        cur_br = BRIGHTNESSMIN;
                }
            }
            //Rock layer
            maxh = min(max(0, h - 5), 16);
            for (y = 0; y < maxh; ++y)
                pblocks[(y << 4) + base] = block(Blocks::ROCK);
            //Air layer
            for (y = min(max(0, max(h + 1, wh + 1)), 16); y < 16; ++y)
            {
                pblocks[(y << 4) + base] = block(Blocks::AIR);
                pbrightness[(y << 4) + base] = skylight;
            }
            //Bedrock layer (overwrite)
            if (cy == 0)
                pblocks[base] = block(Blocks::BEDROCK);
        }
    }
}

void chunk::buildDetail()
{
    int index = 0, x, y, z;
    for (x = 0; x < 16; x++)
    {
        for (y = 0; y < 16; y++)
        {
            for (z = 0; z < 16; z++)
            {
                //Tree
                if (pblocks[index] == block(Blocks::GRASS) && rnd() < 0.005)
                {
                    buildtree(cx * 16 + x, cy * 16 + y, cz * 16 + z);
                }
                index++;
            }
        }
    }
}

void chunk::build(bool initIfEmpty)
{
    buildTerrain(initIfEmpty);
    if (!Empty)
        buildDetail();
}

void chunk::Load(bool initIfEmpty)
{
    //assert(Empty == false);

    create();
#ifndef NEWORLD_DEBUG_NO_FILEIO
    if (!LoadFromFile())
        build(initIfEmpty);
#else
    build(initIfEmpty);
#endif
    if (!Empty) updated = true;
}

void chunk::Unload()
{
    unloadedChunksCount++;
#ifndef NEWORLD_DEBUG_NO_FILEIO
    SaveToFile();
#endif
    destroyRender();
    destroy();
}

bool chunk::LoadFromFile()
{
    std::ifstream file(getChunkPath(), std::ios::in | std::ios::binary);
    bool openChunkFile = file.is_open();
    file.read((char*)pblocks, 4096 * sizeof(block));
    file.read((char*)pbrightness, 4096 * sizeof(brightness));
    file.read((char*)&DetailGenerated, sizeof(bool));
    file.close();

    //file.open(getObjectsPath(), std::ios::in | std::ios::binary);
    //file.close();
    return openChunkFile;
}

void chunk::SaveToFile()
{
    if (!Empty&&Modified)
    {
        std::ofstream file(getChunkPath(), std::ios::out | std::ios::binary);
        file.write((char*)pblocks, 4096 * sizeof(block));
        file.write((char*)pbrightness, 4096 * sizeof(brightness));
        file.write((char*)&DetailGenerated, sizeof(bool));
        file.close();
    }
    if (objects.size() != 0)
    {
        //TODO ?
    }
}

void chunk::buildRender()
{
    int x, y, z;
    for (x = -1; x <= 1; x++)
    {
        for (y = -1; y <= 1; y++)
        {
            for (z = -1; z <= 1; z++)
            {
                if (x == 0 && y == 0 && z == 0)
                    continue;
                if (chunkOutOfBound(cx + x, cy + y, cz + z))
                    continue;
                if (!chunkLoaded(cx + x, cy + y, cz + z))
                    return;
            }
        }
    }

    rebuiltChunks++;
    updatedChunks++;

    if (!renderBuilt)
    {
        renderBuilt = true;
        loadAnim = cy * 16.0f + 16.0f;
    }

    if (MergeFace)
        ChunkRenderer::MergeFaceRender(this);
    else
        ChunkRenderer::RenderChunk(this);

    if (Renderer::AdvancedRender)
        ChunkRenderer::RenderDepthModel(this);

    updated = false;
}

void chunk::destroyRender()
{
    if (!renderBuilt)
        return;
    if (vbuffer[0] != 0)
        vbuffersShouldDelete.push_back(vbuffer[0]);
    if (vbuffer[1] != 0)
        vbuffersShouldDelete.push_back(vbuffer[1]);
    if (vbuffer[2] != 0)
        vbuffersShouldDelete.push_back(vbuffer[2]);
    if (vbuffer[3] != 0)
        vbuffersShouldDelete.push_back(vbuffer[3]);
    vbuffer[0] = vbuffer[1] = vbuffer[2] = vbuffer[3] = 0;
    renderBuilt = false;
}

Hitbox::AABB chunk::getBaseAABB()
{
    Hitbox::AABB ret;
    ret.xmin = cx * 16 - 0.5;
    ret.ymin = cy * 16 - 0.5;
    ret.zmin = cz * 16 - 0.5;
    ret.xmax = cx * 16 + 16 - 0.5;
    ret.ymax = cy * 16 + 16 - 0.5;
    ret.zmax = cz * 16 + 16 - 0.5;
    return ret;
}

Frustum::ChunkBox chunk::getRelativeAABB()
{
    Frustum::ChunkBox ret;
    ret.xmin = (float)(aabb.xmin - relBaseX);
    ret.xmax = (float)(aabb.xmax - relBaseX);
    ret.ymin = (float)(aabb.ymin - loadAnim - relBaseY);
    ret.ymax = (float)(aabb.ymax - loadAnim - relBaseY);
    ret.zmin = (float)(aabb.zmin - relBaseZ);
    ret.zmax = (float)(aabb.zmax - relBaseZ);
    return ret;
}

}
