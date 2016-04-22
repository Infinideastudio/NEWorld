#include "Chunk.h"
#include "WorldGen.h"
#include "World.h"
#include "Blocks.h"
namespace Renderer
{
extern bool AdvancedRender;
}

namespace World
{

double chunk::relBaseX, chunk::relBaseY, chunk::relBaseZ;
Frustum chunk::TestFrustum;

chunk::~chunk()
{
    SaveToFile();
    destroyRender();
    if (World::cpCachePtr == this)
    {
        World::cpCacheID = 0;
        World::cpCachePtr = nullptr;
    }
    delete[] pblocks;
    delete[] pbrightness;
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
        for (size_t i = 0; i < 4096; i++)
            pbrightness[i] = skylight;
        return;
    }
    if (cy < 0)
    {
        Empty = true;
        if (!initIfEmpty)
            return;
        memset(pblocks, 0, 4096 * sizeof(block));
        for (size_t i = 0; i < 4096; i++)
            pbrightness[i] = BRIGHTNESSMIN;
        return;
    }

    //Part2 out of geomentry area

    int H[16][16];
    int low, high, count;
    int l = 2147483647, hi = pWorldGen->getWaterLevel();

    for (int x = 0; x < 16; ++x)
    {
        for (int z = 0; z < 16; ++z)
        {
            int h;
            h = HMap.getHeight(cx * 16 + x, cz * 16 + z);
            if (h < l) l = h;
            if (h > hi) hi = h;
            H[x][z] = h;
        }
    }
    low = (l - 21) >> 4, high = (hi + 16) >> 4;
    count = 0;
    if (cy > high)
    {
        Empty = true;
        if (!initIfEmpty)
            return;
        memset(pblocks, 0, 4096 * sizeof(block));
        memset(pbrightness, 0, 4096 * sizeof(brightness));
        return;
    }
    if (cy < low)
    {
        for (size_t i = 0; i < 4096; i++)
            pblocks[i] = block(Blocks::ROCK);
        memset(pbrightness, 0, 4096 * sizeof(brightness));
        if (cy == 0)
            for (int x = 0; x < 16; x++)
                for (int z = 0; z < 16; z++)
                    pblocks[(x << 8) ^ z] = block(Blocks::BEDROCK);
        Empty = false;
        return;
    }

    //Normal Calc
    //Init
    memset(pblocks, 0, 4096 * sizeof(block)); //Empty the chunk
    memset(pbrightness, 0, 4096 * sizeof(brightness)); //Set All Brightness to 0

    int h = 0, sh = 0, wh = 0, base, y;
    int minh, maxh, cur_br;

    Empty = true;
    sh = pWorldGen->getWaterLevel() + 2 - (cy << 4);
    wh = pWorldGen->getWaterLevel() - (cy << 4);

    for (size_t x = 0; x < 16; ++x)
    {
        for (size_t z = 0; z < 16; ++z)
        {
            base = (x << 8) + z;
            h = H[x][z] - (cy << 4);
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
                cur_br = BRIGHTNESSMAX - (pWorldGen->getWaterLevel() - (maxh - 1 + (cy << 4))) * 2;
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

void chunk::Load(bool initIfEmpty)
{
    std::ifstream file(getChunkFileName(), std::ios::in | std::ios::binary);
    if (file.is_open())
    {
        file.read((char*)pblocks, 4096 * sizeof(block));
        file.read((char*)pbrightness, 4096 * sizeof(brightness));
        file.read((char*)&DetailGenerated, sizeof(bool));
    }
    else
    {
        buildTerrain(initIfEmpty);
        if (!Empty)
        {
            //Build trees
            for (size_t index = 0; index < 4096; index++)
                if (pblocks[index] == block(Blocks::GRASS) && pRandGen->one_in(200))
                    buildtree((cx << 4) ^ ((index&((1 << 12) - (1 << 8))) >> 8), (cy << 4) ^ ((index&((1 << 8) - (1 << 4))) >> 4), (cz << 4) ^ (index & 15));
        }
    }
    if (!Empty) updated = true;
}

// OPTIMIZATION REQUIRED
// recommendation : introduce database to store chunks terrian

void chunk::SaveToFile()
{
    if (!Empty&&Modified)
    {
        std::ofstream file(getChunkFileName(), std::ios::out | std::ios::binary);
        file.write((char*)pblocks, 4096 * sizeof(block));
        file.write((char*)pbrightness, 4096 * sizeof(brightness));
        file.write((char*)&DetailGenerated, sizeof(bool));
    }
}

void chunk::buildRender()
{
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            for (int z = -1; z <= 1; z++)
            {
                if ((x || y || z) && !chunkOutOfBound(cx + x, cy + y, cz + z) && !chunkLoaded(cx + x, cy + y, cz + z))
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
        MergeFaceRender();
    else
        Render();

    if (Renderer::AdvancedRender)
        RenderDepthModel();

    updated = false;
}

void chunk::destroyRender()
{
    if (renderBuilt)
    {
        for (int i = 0; i < 4; i++)
        {
            vbuffersShouldDelete.push_back(vbuffer[i]);
            vbuffer[i] = 0;
        }
        renderBuilt = false;
    }
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
