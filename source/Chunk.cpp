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

#include "Chunk.h"
#include "WorldGen.h"
#include "World.h"
#include "Blocks.h"
#include "Textures.h"
#include "Renderer.h"

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

const int delta[6][3] = { { 1,0,0 },{ -1,0,0 },{ 0,1,0 },{ 0,-1,0 },{ 0,0,1 },{ 0,0,-1 } };

//合并面的一整个面 | One face in merge face
struct QuadPrimitive
{
    int x, y, z, length, direction;
    /*
    * 如果顶点颜色不同（平滑光照启用时），这个方形就不能和别的方形拼合起来。
    * 这个变量同时意味着四个顶点颜色是否不同。
    * If the vertexes have different colors (smoothed lighting), the primitive cannot connect with others.
    * This variable also means whether the vertexes have different colors.
    */
    bool once;
    //顶点颜色 | Vertex colors
    int col0, col1, col2, col3;
    //纹理ID | Texture ID
    TextureID tex;
    QuadPrimitive() : x(0), y(0), z(0), length(0), direction(0), once(false),
        tex(Textures::NULLBLOCK), col0(0), col1(0), col2(0), col3(0) {}
};

//深度模型的面 | Face in depth model
struct QuadPrimitive_Depth
{
    int x, y, z, length, direction;
    QuadPrimitive_Depth() : x(0), y(0), z(0), length(0), direction(0) {}
};
/*
合并面的顶点顺序（以0到3标出）：

The vertex order of merge face render
Numbered from 0 to 3:

(k++)
...
|    |
+----+--
|    |
|    |    |
3----2----+-
|curr|    |   ...
|face|    |   (j++)
0----1----+--

--qiaozhanrong
*/

void RenderPrimitive(QuadPrimitive& p)
{
    float col0 = (float)p.col0 * 0.25f / World::BRIGHTNESSMAX;
    float col1 = (float)p.col1 * 0.25f / World::BRIGHTNESSMAX;
    float col2 = (float)p.col2 * 0.25f / World::BRIGHTNESSMAX;
    float col3 = (float)p.col3 * 0.25f / World::BRIGHTNESSMAX;
    int x = p.x, y = p.y, z = p.z, length = p.length;
#ifdef NERDMODE1
    Renderer::TexCoord3d(0.0, 0.0, (p.tex + 0.5) / 64.0);
    if (p.direction == 0)
    {
        if (Renderer::AdvancedRender) Renderer::Attrib1f(2.0f);
        else col0 *= 0.7f, col1 *= 0.7f, col2 *= 0.7f, col3 *= 0.7f;
        Renderer::Color3f(col0, col0, col0);
        Renderer::TexCoord2d(0.0, 0.0);
        Renderer::Vertex3d(x + 0.5, y - 0.5, z - 0.5);
        Renderer::Color3f(col1, col1, col1);
        Renderer::TexCoord2d(0.0, 1.0);
        Renderer::Vertex3d(x + 0.5, y + 0.5, z - 0.5);
        Renderer::Color3f(col2, col2, col2);
        Renderer::TexCoord2d(length + 1.0, 1.0);
        Renderer::Vertex3d(x + 0.5, y + 0.5, z + length + 0.5);
        Renderer::Color3f(col3, col3, col3);
        Renderer::TexCoord2d(length + 1.0, 0.0);
        Renderer::Vertex3d(x + 0.5, y - 0.5, z + length + 0.5);
    }
    else if (p.direction == 1)
    {
        if (Renderer::AdvancedRender) Renderer::Attrib1f(3.0f);
        else col0 *= 0.7f, col1 *= 0.7f, col2 *= 0.7f, col3 *= 0.7f;
        Renderer::Color3f(col0, col0, col0);
        Renderer::TexCoord2d(0.0, 1.0);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z - 0.5);
        Renderer::Color3f(col1, col1, col1);
        Renderer::TexCoord2d(0.0, 0.0);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z - 0.5);
        Renderer::Color3f(col2, col2, col2);
        Renderer::TexCoord2d(length + 1.0, 0.0);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z + length + 0.5);
        Renderer::Color3f(col3, col3, col3);
        Renderer::TexCoord2d(length + 1.0, 1.0);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z + length + 0.5);
    }
    else if (p.direction == 2)
    {
        if (Renderer::AdvancedRender) Renderer::Attrib1f(4.0f);
        Renderer::Color3f(col0, col0, col0);
        Renderer::TexCoord2d(0.0, 0.0);
        Renderer::Vertex3d(x + 0.5, y + 0.5, z - 0.5);
        Renderer::Color3f(col1, col1, col1);
        Renderer::TexCoord2d(0.0, 1.0);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z - 0.5);
        Renderer::Color3f(col2, col2, col2);
        Renderer::TexCoord2d(length + 1.0, 1.0);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z + length + 0.5);
        Renderer::Color3f(col3, col3, col3);
        Renderer::TexCoord2d(length + 1.0, 0.0);
        Renderer::Vertex3d(x + 0.5, y + 0.5, z + length + 0.5);
    }
    else if (p.direction == 3)
    {
        if (Renderer::AdvancedRender) Renderer::Attrib1f(5.0f);
        Renderer::Color3f(col0, col0, col0);
        Renderer::TexCoord2d(0.0, 0.0);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z - 0.5);
        Renderer::Color3f(col1, col1, col1);
        Renderer::TexCoord2d(0.0, 1.0);
        Renderer::Vertex3d(x + 0.5, y - 0.5, z - 0.5);
        Renderer::Color3f(col2, col2, col2);
        Renderer::TexCoord2d(length + 1.0, 1.0);
        Renderer::Vertex3d(x + 0.5, y - 0.5, z + length + 0.5);
        Renderer::Color3f(col3, col3, col3);
        Renderer::TexCoord2d(length + 1.0, 0.0);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z + length + 0.5);
    }
    else if (p.direction == 4)
    {
        if (Renderer::AdvancedRender) Renderer::Attrib1f(0.0f);
        else col0 *= 0.5f, col1 *= 0.5f, col2 *= 0.5f, col3 *= 0.5f;
        Renderer::Color3f(col0, col0, col0);
        Renderer::TexCoord2d(0.0, 1.0);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z + 0.5);
        Renderer::Color3f(col1, col1, col1);
        Renderer::TexCoord2d(0.0, 0.0);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z + 0.5);
        Renderer::Color3f(col2, col2, col2);
        Renderer::TexCoord2d(length + 1.0, 0.0);
        Renderer::Vertex3d(x + length + 0.5, y - 0.5, z + 0.5);
        Renderer::Color3f(col3, col3, col3);
        Renderer::TexCoord2d(length + 1.0, 1.0);
        Renderer::Vertex3d(x + length + 0.5, y + 0.5, z + 0.5);
    }
    else if (p.direction == 5)
    {
        if (Renderer::AdvancedRender) Renderer::Attrib1f(1.0f);
        else col0 *= 0.5f, col1 *= 0.5f, col2 *= 0.5f, col3 *= 0.5f;
        Renderer::Color3f(col0, col0, col0);
        Renderer::TexCoord2d(0.0, 0.0);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z - 0.5);
        Renderer::Color3f(col1, col1, col1);
        Renderer::TexCoord2d(0.0, 1.0);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z - 0.5);
        Renderer::Color3f(col2, col2, col2);
        Renderer::TexCoord2d(length + 1.0, 1.0);
        Renderer::Vertex3d(x + length + 0.5, y + 0.5, z - 0.5);
        Renderer::Color3f(col3, col3, col3);
        Renderer::TexCoord2d(length + 1.0, 0.0);
        Renderer::Vertex3d(x + length + 0.5, y - 0.5, z - 0.5);
    }
#else
    float T3d = (Textures::getTextureIndex(p.block, face) - 0.5) / 64.0;
    switch (p.direction)
    {
    case 0:
    {
        if (p.block != block(Blocks::GLOWSTONE)) color *= 0.7;
        float geomentry[] =
        {
            0.0, 0.0, T3d, color, color, color, x + 0.5, y - 0.5, z - 0.5,
            0.0, 1.0, T3d, color, color, color, x + 0.5, y + 0.5, z - 0.5,
            length + 1.0, 1.0, T3d, color, color, color, x + 0.5, y + 0.5, z + length + 0.5,
            length + 1.0, 0.0, T3d, color, color, color, x + 0.5, y - 0.5, z + length + 0.5
        };
        Renderer::Quad(geomentry);
    }
    break;
    case 1:
    {
        if (p.block != block(Blocks::GLOWSTONE)) color *= 0.7;
        float geomentry[] =
        {
            0.0, 1.0, T3d, color, color, color, x - 0.5, y + 0.5, z - 0.5,
            0.0, 0.0, T3d, color, color, color, x - 0.5, y - 0.5, z - 0.5,
            length + 1.0, 0.0, T3d, color, color, color, x - 0.5, y - 0.5, z + length + 0.5,
            length + 1.0, 1.0, T3d, color, color, color, x - 0.5, y + 0.5, z + length + 0.5
        };
        Renderer::Quad(geomentry);
    }
    break;
    case 2:
    {
        float geomentry[] =
        {
            0.0, 0.0, T3d, color, color, color, x + 0.5, y + 0.5, z - 0.5,
            0.0, 1.0, T3d, color, color, color, x - 0.5, y + 0.5, z - 0.5,
            length + 1.0, 1.0, T3d, color, color, color, x - 0.5, y + 0.5, z + length + 0.5,
            length + 1.0, 0.0, T3d, color, color, color, x + 0.5, y + 0.5, z + length + 0.5
        };
        Renderer::Quad(geomentry);
    }
    break;
    case 3:
    {
        float geomentry[] =
        {
            0.0, 0.0, T3d, color, color, color, x - 0.5, y - 0.5, z - 0.5,
            0.0, 1.0, T3d, color, color, color, x + 0.5, y - 0.5, z - 0.5,
            length + 1.0, 1.0, T3d, color, color, color, x + 0.5, y - 0.5, z + length + 0.5,
            length + 1.0, 0.0, T3d, color, color, color, x - 0.5, y - 0.5, z + length + 0.5
        };
        Renderer::Quad(geomentry);
    }
    break;
    case 4:
    {
        if (p.block != block(Blocks::GLOWSTONE)) color *= 0.5;
        float geomentry[] =
        {
            0.0, 1.0, T3d, color, color, color, x - 0.5, y + 0.5, z + 0.5,
            0.0, 0.0, T3d, color, color, color, x - 0.5, y - 0.5, z + 0.5,
            length + 1.0, 0.0, T3d, color, color, color, x + length + 0.5, y - 0.5, z + 0.5,
            length + 1.0, 1.0, T3d, color, color, color, x + length + 0.5, y + 0.5, z + 0.5
        };
        Renderer::Quad(geomentry);
    }
    break;
    case 5:
    {
        if (p.block != block(Blocks::GLOWSTONE)) color *= 0.5;
        float geomentry[] =
        {
            0.0, 0.0, T3d, color, color, color, x - 0.5, y - 0.5, z - 0.5,
            0.0, 1.0, T3d, color, color, color, x - 0.5, y + 0.5, z - 0.5,
            length + 1.0, 1.0, T3d, color, color, color, x + length + 0.5, y + 0.5, z - 0.5,
            length + 1.0, 0.0, T3d, color, color, color, x + length + 0.5, y - 0.5, z - 0.5
        };
        Renderer::Quad(geomentry);
    }
    break;
    }
#endif // NERDMODE1

}

void RenderPrimitive_Depth(QuadPrimitive_Depth& p)
{
    int x = p.x, y = p.y, z = p.z, length = p.length;
    if (p.direction == 0)
    {
        Renderer::Vertex3d(x + 0.5, y - 0.5, z - 0.5);
        Renderer::Vertex3d(x + 0.5, y + 0.5, z - 0.5);
        Renderer::Vertex3d(x + 0.5, y + 0.5, z + length + 0.5);
        Renderer::Vertex3d(x + 0.5, y - 0.5, z + length + 0.5);
    }
    else if (p.direction == 1)
    {
        Renderer::Vertex3d(x - 0.5, y + 0.5, z - 0.5);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z - 0.5);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z + length + 0.5);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z + length + 0.5);
    }
    else if (p.direction == 2)
    {
        Renderer::Vertex3d(x + 0.5, y + 0.5, z - 0.5);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z - 0.5);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z + length + 0.5);
        Renderer::Vertex3d(x + 0.5, y + 0.5, z + length + 0.5);
    }
    else if (p.direction == 3)
    {
        Renderer::Vertex3d(x - 0.5, y - 0.5, z - 0.5);
        Renderer::Vertex3d(x + 0.5, y - 0.5, z - 0.5);
        Renderer::Vertex3d(x + 0.5, y - 0.5, z + length + 0.5);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z + length + 0.5);
    }
    else if (p.direction == 4)
    {
        Renderer::Vertex3d(x - 0.5, y + 0.5, z + 0.5);
        Renderer::Vertex3d(x - 0.5, y - 0.5, z + 0.5);
        Renderer::Vertex3d(x + length + 0.5, y - 0.5, z + 0.5);
        Renderer::Vertex3d(x + length + 0.5, y + 0.5, z + 0.5);
    }
    else if (p.direction == 5)
    {
        Renderer::Vertex3d(x - 0.5, y - 0.5, z - 0.5);
        Renderer::Vertex3d(x - 0.5, y + 0.5, z - 0.5);
        Renderer::Vertex3d(x + length + 0.5, y + 0.5, z - 0.5);
        Renderer::Vertex3d(x + length + 0.5, y - 0.5, z - 0.5);
    }
}

void chunk::Render()
{
    int x, y, z;
    if (Renderer::AdvancedRender) Renderer::Init(2, 3, 1);
    else Renderer::Init(2, 3);
    for (x = 0; x < 16; x++)
    {
        for (y = 0; y < 16; y++)
        {
            for (z = 0; z < 16; z++)
            {
                block curr = getblock(x, y, z);
                if (curr == block(Blocks::AIR)) continue;
                if (!BlockInfo(curr).isTranslucent()) renderblock(x, y, z, this);
            }
        }
    }
    Renderer::Flush(vbuffer[0], vertexes[0]);
    if (Renderer::AdvancedRender) Renderer::Init(2, 3, 1);
    else Renderer::Init(2, 3);
    for (x = 0; x < 16; x++)
    {
        for (y = 0; y < 16; y++)
        {
            for (z = 0; z < 16; z++)
            {
                block curr = getblock(x, y, z);
                if (curr == block(Blocks::AIR)) continue;
                if (BlockInfo(curr).isTranslucent() && BlockInfo(curr).isSolid()) renderblock(x, y, z, this);
            }
        }
    }
    Renderer::Flush(vbuffer[1], vertexes[1]);
    if (Renderer::AdvancedRender) Renderer::Init(2, 3, 1);
    else Renderer::Init(2, 3);
    for (x = 0; x < 16; x++)
    {
        for (y = 0; y < 16; y++)
        {
            for (z = 0; z < 16; z++)
            {
                block curr = getblock(x, y, z);
                if (curr == block(Blocks::AIR)) continue;
                if (!BlockInfo(curr).isSolid()) renderblock(x, y, z, this);
            }
        }
    }
    Renderer::Flush(vbuffer[2], vertexes[2]);
}

//合并面大法好！！！
void chunk::MergeFaceRender()
{
    //话说我注释一会中文一会英文是不是有点奇怪。。。
    // -- qiaozhanrong

    int gx = 0, gy = 0, gz = 0;
    int x = 0, y = 0, z = 0, cur_l_mx, br;
    int col0 = 0, col1 = 0, col2 = 0, col3 = 0;
    QuadPrimitive cur;
    block bl, neighbour;
    ubyte face = 0;
    TextureID tex;
    bool valid = false;
    for (int steps = 0; steps < 3; steps++)
    {
        cur = QuadPrimitive();
        cur_l_mx = 0;
        bl = neighbour = block(0);
        //Linear merge
        if (Renderer::AdvancedRender) Renderer::Init(3, 3, 1);
        else Renderer::Init(3, 3);
        for (int d = 0; d < 6; d++)
        {
            cur.direction = d;
            if (d == 2) face = 1;
            else if (d == 3) face = 3;
            else face = 2;
            //Render current face
            for (int i = 0; i < 16; i++)
                for (int j = 0; j < 16; j++)
                {
                    for (int k = 0; k < 16; k++)
                    {
                        //Get position & brightness
                        if (d == 0)   //x+
                        {
                            x = i, y = j, z = k;
                            gx = cx * 16 + x;
                            gy = cy * 16 + y;
                            gz = cz * 16 + z;
                            br = World::getbrightness(gx + 1, gy, gz, this);
                            if (SmoothLighting)
                            {
                                col0 = br + World::getbrightness(gx + 1, gy - 1, gz, this) + World::getbrightness(gx + 1, gy, gz - 1, this) + World::getbrightness(gx + 1, gy - 1, gz - 1, this);
                                col1 = br + World::getbrightness(gx + 1, gy + 1, gz, this) + World::getbrightness(gx + 1, gy, gz - 1, this) + World::getbrightness(gx + 1, gy + 1, gz - 1, this);
                                col2 = br + World::getbrightness(gx + 1, gy + 1, gz, this) + World::getbrightness(gx + 1, gy, gz + 1, this) + World::getbrightness(gx + 1, gy + 1, gz + 1, this);
                                col3 = br + World::getbrightness(gx + 1, gy - 1, gz, this) + World::getbrightness(gx + 1, gy, gz + 1, this) + World::getbrightness(gx + 1, gy - 1, gz + 1, this);
                            }
                            else col0 = col1 = col2 = col3 = br * 4;
                        }
                        else if (d == 1)   //x-
                        {
                            x = i, y = j, z = k;
                            gx = cx * 16 + x;
                            gy = cy * 16 + y;
                            gz = cz * 16 + z;
                            br = World::getbrightness(gx - 1, gy, gz, this);
                            if (SmoothLighting)
                            {
                                col0 = br + World::getbrightness(gx - 1, gy + 1, gz, this) + World::getbrightness(gx - 1, gy, gz - 1, this) + World::getbrightness(gx - 1, gy + 1, gz - 1, this);
                                col1 = br + World::getbrightness(gx - 1, gy - 1, gz, this) + World::getbrightness(gx - 1, gy, gz - 1, this) + World::getbrightness(gx - 1, gy - 1, gz - 1, this);
                                col2 = br + World::getbrightness(gx - 1, gy - 1, gz, this) + World::getbrightness(gx - 1, gy, gz + 1, this) + World::getbrightness(gx - 1, gy - 1, gz + 1, this);
                                col3 = br + World::getbrightness(gx - 1, gy + 1, gz, this) + World::getbrightness(gx - 1, gy, gz + 1, this) + World::getbrightness(gx - 1, gy + 1, gz + 1, this);
                            }
                            else col0 = col1 = col2 = col3 = br * 4;
                        }
                        else if (d == 2)   //y+
                        {
                            x = j, y = i, z = k;
                            gx = cx * 16 + x;
                            gy = cy * 16 + y;
                            gz = cz * 16 + z;
                            br = World::getbrightness(gx, gy + 1, gz, this);
                            if (SmoothLighting)
                            {
                                col0 = br + World::getbrightness(gx + 1, gy + 1, gz, this) + World::getbrightness(gx, gy + 1, gz - 1, this) + World::getbrightness(gx + 1, gy + 1, gz - 1, this);
                                col1 = br + World::getbrightness(gx - 1, gy + 1, gz, this) + World::getbrightness(gx, gy + 1, gz - 1, this) + World::getbrightness(gx - 1, gy + 1, gz - 1, this);
                                col2 = br + World::getbrightness(gx - 1, gy + 1, gz, this) + World::getbrightness(gx, gy + 1, gz + 1, this) + World::getbrightness(gx - 1, gy + 1, gz + 1, this);
                                col3 = br + World::getbrightness(gx + 1, gy + 1, gz, this) + World::getbrightness(gx, gy + 1, gz + 1, this) + World::getbrightness(gx + 1, gy + 1, gz + 1, this);
                            }
                            else col0 = col1 = col2 = col3 = br * 4;
                        }
                        else if (d == 3)   //y-
                        {
                            x = j, y = i, z = k;
                            gx = cx * 16 + x;
                            gy = cy * 16 + y;
                            gz = cz * 16 + z;
                            br = World::getbrightness(gx, gy - 1, gz, this);
                            if (SmoothLighting)
                            {
                                col0 = br + World::getbrightness(gx - 1, gy - 1, gz, this) + World::getbrightness(gx, gy - 1, gz - 1, this) + World::getbrightness(gx - 1, gy - 1, gz - 1, this);
                                col1 = br + World::getbrightness(gx + 1, gy - 1, gz, this) + World::getbrightness(gx, gy - 1, gz - 1, this) + World::getbrightness(gx + 1, gy - 1, gz - 1, this);
                                col2 = br + World::getbrightness(gx + 1, gy - 1, gz, this) + World::getbrightness(gx, gy - 1, gz + 1, this) + World::getbrightness(gx + 1, gy - 1, gz + 1, this);
                                col3 = br + World::getbrightness(gx - 1, gy - 1, gz, this) + World::getbrightness(gx, gy - 1, gz + 1, this) + World::getbrightness(gx - 1, gy - 1, gz + 1, this);
                            }
                            else col0 = col1 = col2 = col3 = br * 4;
                        }
                        else if (d == 4)   //z+
                        {
                            x = k, y = j, z = i;
                            gx = cx * 16 + x;
                            gy = cy * 16 + y;
                            gz = cz * 16 + z;
                            br = World::getbrightness(gx, gy, gz + 1, this);
                            if (SmoothLighting)
                            {
                                col0 = br + World::getbrightness(gx - 1, gy, gz + 1, this) + World::getbrightness(gx, gy + 1, gz + 1, this) + World::getbrightness(gx - 1, gy + 1, gz + 1, this);
                                col1 = br + World::getbrightness(gx - 1, gy, gz + 1, this) + World::getbrightness(gx, gy - 1, gz + 1, this) + World::getbrightness(gx - 1, gy - 1, gz + 1, this);
                                col2 = br + World::getbrightness(gx + 1, gy, gz + 1, this) + World::getbrightness(gx, gy - 1, gz + 1, this) + World::getbrightness(gx + 1, gy - 1, gz + 1, this);
                                col3 = br + World::getbrightness(gx + 1, gy, gz + 1, this) + World::getbrightness(gx, gy + 1, gz + 1, this) + World::getbrightness(gx + 1, gy + 1, gz + 1, this);
                            }
                            else col0 = col1 = col2 = col3 = br * 4;
                        }
                        else if (d == 5)   //z-
                        {
                            x = k, y = j, z = i;
                            gx = cx * 16 + x;
                            gy = cy * 16 + y;
                            gz = cz * 16 + z;
                            br = World::getbrightness(gx, gy, gz - 1, this);
                            if (SmoothLighting)
                            {
                                col0 = br + World::getbrightness(gx - 1, gy, gz - 1, this) + World::getbrightness(gx, gy - 1, gz - 1, this) + World::getbrightness(gx - 1, gy - 1, gz - 1, this);
                                col1 = br + World::getbrightness(gx - 1, gy, gz - 1, this) + World::getbrightness(gx, gy + 1, gz - 1, this) + World::getbrightness(gx - 1, gy + 1, gz - 1, this);
                                col2 = br + World::getbrightness(gx + 1, gy, gz - 1, this) + World::getbrightness(gx, gy + 1, gz - 1, this) + World::getbrightness(gx + 1, gy + 1, gz - 1, this);
                                col3 = br + World::getbrightness(gx + 1, gy, gz - 1, this) + World::getbrightness(gx, gy - 1, gz - 1, this) + World::getbrightness(gx + 1, gy - 1, gz - 1, this);
                            }
                            else col0 = col1 = col2 = col3 = br * 4;
                        }
                        //Get block ID
                        bl = getblock(x, y, z);
                        tex = Textures::getTextureIndex(bl, face);
                        neighbour = World::getblock(gx + delta[d][0], gy + delta[d][1], gz + delta[d][2], block(Blocks::ROCK), this);
                        if (NiceGrass && bl == block(Blocks::GRASS))
                        {
                            if (d == 0 && World::getblock(gx + 1, gy - 1, gz, block(Blocks::ROCK), this) == block(Blocks::GRASS)) tex = Textures::getTextureIndex(bl, 1);
                            else if (d == 1 && World::getblock(gx - 1, gy - 1, gz, block(Blocks::ROCK), this) == block(Blocks::GRASS)) tex = Textures::getTextureIndex(bl, 1);
                            else if (d == 4 && World::getblock(gx, gy - 1, gz + 1, block(Blocks::ROCK), this) == block(Blocks::GRASS)) tex = Textures::getTextureIndex(bl, 1);
                            else if (d == 5 && World::getblock(gx, gy - 1, gz - 1, block(Blocks::ROCK), this) == block(Blocks::GRASS)) tex = Textures::getTextureIndex(bl, 1);
                        }
                        //Render
                        const Blocks::SingleBlock& info = BlockInfo(bl);
                        if (bl == block(Blocks::AIR) || (bl == neighbour && bl != block(Blocks::LEAF)) || BlockInfo(neighbour).isOpaque() ||
                                (steps == 0 && info.isTranslucent()) ||
                                (steps == 1 && (!info.isTranslucent() || !info.isSolid())) ||
                                (steps == 2 && (!info.isTranslucent() || info.isSolid())))
                        {
                            //Not valid block
                            if (valid)
                            {
                                if (BlockInfo(neighbour).isOpaque() && !cur.once)
                                {
                                    if (cur_l_mx < cur.length) cur_l_mx = cur.length;
                                    cur_l_mx++;
                                }
                                else
                                {
                                    RenderPrimitive(cur);
                                    valid = false;
                                }
                            }
                            continue;
                        }
                        if (valid)
                        {
                            if (col0 != col1 || col1 != col2 || col2 != col3 || cur.once || tex != cur.tex || col0 != cur.col0)
                            {
                                RenderPrimitive(cur);
                                cur.x = x;
                                cur.y = y;
                                cur.z = z;
                                cur.length = cur_l_mx = 0;
                                cur.tex = tex;
                                cur.col0 = col0;
                                cur.col1 = col1;
                                cur.col2 = col2;
                                cur.col3 = col3;
                                if (col0 != col1 || col1 != col2 || col2 != col3) cur.once = true;
                                else cur.once = false;
                            }
                            else
                            {
                                if (cur_l_mx > cur.length) cur.length = cur_l_mx;
                                cur.length++;
                            }
                        }
                        else
                        {
                            valid = true;
                            cur.x = x;
                            cur.y = y;
                            cur.z = z;
                            cur.length = cur_l_mx = 0;
                            cur.tex = tex;
                            cur.col0 = col0;
                            cur.col1 = col1;
                            cur.col2 = col2;
                            cur.col3 = col3;
                            if (col0 != col1 || col1 != col2 || col2 != col3) cur.once = true;
                            else cur.once = false;
                        }
                    }
                    if (valid)
                    {
                        RenderPrimitive(cur);
                        valid = false;
                    }
                }
        }
        Renderer::Flush(vbuffer[steps], vertexes[steps]);
    }
}

void chunk::RenderDepthModel()
{
    int x = 0, y = 0, z = 0;
    QuadPrimitive_Depth cur;
    int cur_l_mx;
    block bl, neighbour;
    bool valid = false;
    cur_l_mx = 0;
    bl = neighbour = block(0);
    //Linear merge for depth model
    Renderer::Init(0, 0);
    for (int d = 0; d < 6; d++)
    {
        cur.direction = d;
        for (int i = 0; i < 16; i++)
            for (int j = 0; j < 16; j++)
            {
                for (int k = 0; k < 16; k++)
                {
                    //Get position
                    if (d < 2) x = i, y = j, z = k;
                    else if (d < 4) x = i, y = j, z = k;
                    else x = k, y = i, z = j;
                    //Get block ID
                    bl = getblock(x, y, z);
                    //Get neighbour ID
                    int xx = x + delta[d][0], yy = y + delta[d][1], zz = z + delta[d][2];
                    int gx = cx * 16 + xx, gy = cy * 16 + yy, gz = cz * 16 + zz;
                    if (xx < 0 || xx >= 16 || yy < 0 || yy >= 16 || zz < 0 || zz >= 16)
                        neighbour = World::getblock(gx, gy, gz);
                    else neighbour = getblock(xx, yy, zz);
                    //Render
                    if (bl == block(Blocks::AIR) || bl == block(Blocks::GLASS) || (bl == neighbour && bl != block(Blocks::LEAF)) ||
                            BlockInfo(neighbour).isOpaque() || BlockInfo(bl).isTranslucent())
                    {
                        //Not valid block
                        if (valid)
                        {
                            if (BlockInfo(neighbour).isOpaque())
                            {
                                if (cur_l_mx < cur.length) cur_l_mx = cur.length;
                                cur_l_mx++;
                            }
                            else
                            {
                                RenderPrimitive_Depth(cur);
                                valid = false;
                            }
                        }
                        continue;
                    }
                    if (valid)
                    {
                        if (cur_l_mx > cur.length) cur.length = cur_l_mx;
                        cur.length++;
                    }
                    else
                    {
                        valid = true;
                        cur.x = x;
                        cur.y = y;
                        cur.z = z;
                        cur.length = cur_l_mx = 0;
                    }
                }
                if (valid)
                {
                    RenderPrimitive_Depth(cur);
                    valid = false;
                }
            }
    }
    Renderer::Flush(vbuffer[3], vertexes[3]);
}
}
