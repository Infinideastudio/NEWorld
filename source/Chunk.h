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

#ifndef CHUNK_H
#define CHUNK_H

#include "Definitions.h"
#include "Hitbox.h"
#include "Blocks.h"
#include "Frustum.h"

class Object;

namespace World
{

void MarkBlockUpdate(Blocks::BUDDP Block);
extern string Name;
extern brightness BRIGHTNESSMIN;
extern brightness skylight;

class chunk;
chunkid getChunkID(int x, int y, int z);
void explode(int x, int y, int z, int r, chunk* c);

class chunk
{
private:
    brightness* pbrightness;
    vector<Object*> objects;
    static double relBaseX, relBaseY, relBaseZ;
    static Frustum TestFrustum;

public:
    block* pblocks; //moved here for blockupd would get block ptr directly
    chunk(int cxi, int cyi, int czi, chunkid idi) : cx(cxi), cy(cyi), cz(czi), id(idi),
        Modified(false), Empty(false), updated(false), renderBuilt(false), loadAnim(0.0)
    {
        memset(vertexes, 0, sizeof(vertexes));
        memset(vbuffer, 0, sizeof(vbuffer));
        aabb = getBaseAABB();
        pblocks = new block[4096];
        pbrightness = new brightness[4096];
    }

    int cx, cy, cz;
    Hitbox::AABB aabb;
    bool Empty, updated, renderBuilt, Modified;
    chunkid id;
    vtxCount vertexes[4];
    VBOID vbuffer[4];
    double loadAnim;
    bool visible;

    ~chunk();
    void Load(bool initIfEmpty = true);

    string getChunkFileName()
    {
        std::stringstream ss;
        ss << "Worlds/" << Name << "/chunks/chunk_" << cx << "_" << cy << "_" << cz << ".NEWorldChunk";
        return ss.str();
    }

    void SaveToFile();
    void buildRender();
    void destroyRender();

    // Member functions implemented inside the declaration of a class
    // are regarded as inline functions by default. Please learn
    // the correct usage of C++ programming language.
    block& getblock(int x, int y, int z)
    {
#ifdef _DEBUG
        assert(x >= 0 && x < 16 && y >= 0 && y < 16 && z >= 0 && z < 16);
#endif
        return pblocks[(x << 8) ^ (y << 4) ^ z];
    }

    brightness getbrightness(int x, int y, int z)
    {
#ifdef _DEBUG
        assert(x >= 0 && x < 16 && y >= 0 && y < 16 && z >= 0 && z < 16);
#endif
        return pbrightness[(x << 8) ^ (y << 4) ^ z];
    }

    void setblock(int x, int y, int z, block iblock)
    {
        if (iblock == block(Blocks::TNT))
        {
            World::explode(cx * 16 + x, cy * 16 + y, cz * 16 + z, 8, this);
            return;
        }
        pblocks[(x << 8) ^ (y << 4) ^ z] = iblock;
        Modified = true;
    }

    void Modifyblock(int x, int y, int z, block iblock)
    {
        if (iblock == block(Blocks::TNT))
        {
            World::explode(cx * 16 + x, cy * 16 + y, cz * 16 + z, 8, this);
            return;
        }
        MarkBlockUpdate(Blocks::BUDDP(*(pblocks + ((x << 8) ^ (y << 4) ^ z)), pblocks + ((x << 8) ^ (y << 4) ^ z), nullptr, nullptr, nullptr, cx * 16 + x, cy * 16 + y, cz * 16 + z));
        pblocks[(x << 8) ^ (y << 4) ^ z] = iblock;
        Modified = true;
    }

    void setbrightness(int x, int y, int z, brightness ibrightness)
    {
        pbrightness[(x << 8) ^ (y << 4) ^ z] = ibrightness;
        Modified = true;
    }

    static void setRelativeBase(double x, double y, double z, Frustum& frus)
    {
        relBaseX = x;
        relBaseY = y;
        relBaseZ = z;
        TestFrustum = frus;
    }

    Hitbox::AABB getBaseAABB();
    Frustum::ChunkBox getRelativeAABB();
    void calcVisible()
    {
        visible = TestFrustum.FrustumTest(getRelativeAABB());
    }

    //Rendering Functions
    void Render();
    void MergeFaceRender();
    void RenderDepthModel();
};
}

#endif
