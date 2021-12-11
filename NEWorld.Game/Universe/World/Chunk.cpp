#include "Chunk.h"
#include "WorldGen.h"
#include "World.h"
#include "Blocks.h"
#include <limits>
#include <fstream>

namespace ChunkRenderer {
    void RenderChunk(World::Chunk *c);

    void MergeFaceRender(World::Chunk *c);

    void RenderDepthModel(World::Chunk *c);
}

namespace Renderer {
    extern bool AdvancedRender;
}

namespace {
    World::ChunkGenerator gChunkGenerator = nullptr;
}

namespace World {
    void UseChunkGenerator(const ChunkGenerator newGenerator) noexcept {
        gChunkGenerator = newGenerator;
    }

    struct HMapManager {
        int H[16][16];
        int low, high, count;

        HMapManager() {};

        HMapManager(int cx, int cz) {
            auto l = std::numeric_limits<int>::max(), hi = WorldGen::WaterLevel;
            for (auto x = 0; x < 16; ++x) {
                for (auto z = 0; z < 16; ++z) {
                    const auto h = HMap.getHeight(cx * 16 + x, cz * 16 + z);
                    if (h < l) l = h;
                    if (h > hi) hi = h;
                    H[x][z] = h;
                }
            }
            low = (l - 21) / 16, high = (hi + 16) / 16;
            count = 0;
        }
    };


    double Chunk::relBaseX, Chunk::relBaseY, Chunk::relBaseZ;
    Frustum Chunk::TestFrustum;

    void Chunk::create() {
        aabb = getBaseAABB();
        mBrightness = new Brightness[4096];
    }

    void Chunk::destroy() {
        //HMapExclude(cx, cz);
        delete[] mBrightness;
        mBrightness = nullptr;
        updated = false;
        unloadedChunks++;
    }

    void Chunk::buildTerrain(bool initIfEmpty) {
        //Fast generate parts
        //Part1 out of the terrain bound
        if (cy > 4) {
            Empty = true;
            if (!initIfEmpty) return;
            for (auto i = 0; i < 4096; i++) {
                mBlock.Set(i, Blocks::ENV);
                mBrightness[i] = skylight;
            }
            return;
        }
        if (cy < 0) {
            Empty = true;
            if (!initIfEmpty) return;
            for (auto i = 0; i < 4096; i++) {
                mBlock.Set(i, Blocks::ENV);
                mBrightness[i] = BRIGHTNESSMIN;
            }
            return;
        }

        //Part2 out of geomentry area
        auto cur = HMapManager(cx, cz);
        if (cy > cur.high) {
            Empty = true;
            if (!initIfEmpty) return;
            for (auto i = 0; i < 4096; i++) {
                mBlock.Set(i, Blocks::ENV);
                mBrightness[i] = skylight;
            }
            return;
        }
        if (cy < cur.low) {
            for (auto i = 0; i < 4096; i++) mBlock.Set(i, Blocks::ROCK);
            memset(mBrightness, 0, 4096 * sizeof(Brightness));
            if (cy == 0)
                for (auto x = 0; x < 16; x++)
                    for (auto z = 0; z < 16; z++)
                        mBlock.Set(x * 256 + z, Blocks::BEDROCK);
            Empty = false;
            return;
        }

        //Normal Calc
        //Init
        memset(mBrightness, 0, 4096 * sizeof(Brightness)); //Set All Brightness to 0

        auto h = 0, sh = 0, wh = 0;
        int maxh;

        Empty = true;
        sh = WorldGen::WaterLevel + 2 - (cy << 4);
        wh = WorldGen::WaterLevel - (cy << 4);

        for (auto x = 0; x < 16; ++x) {
            for (auto z = 0; z < 16; ++z) {
                const auto base = (x << 8) + z;
                h = cur.H[x][z] - (cy << 4);
                if (h >= 0 || wh >= 0) Empty = false;
                if (h > sh && h > wh + 1) {
                    //Grass layer
                    if (h >= 0 && h < 16) mBlock.Set((h << 4) + base, Blocks::GRASS);
                    //Dirt layer
                    maxh = std::min(std::max(0, h), 16);
                    for (auto y = std::min(std::max(0, h - 5), 16); y < maxh; ++y)
                        mBlock.Set((y << 4) + base, Blocks::DIRT);
                } else {
                    //Sand layer
                    maxh = std::min(std::max(0, h + 1), 16);
                    for (auto y = std::min(std::max(0, h - 5), 16); y < maxh; ++y)
                        mBlock.Set((y << 4) + base, Blocks::SAND);
                    //Water layer
                    const auto minh = std::min(std::max(0, h + 1), 16);
                    maxh = std::min(std::max(0, wh + 1), 16);
                    auto cur_br = BRIGHTNESSMAX - (WorldGen::WaterLevel - (maxh - 1 + (cy << 4))) * 2;
                    if (cur_br < BRIGHTNESSMIN) cur_br = BRIGHTNESSMIN;
                    for (auto y = maxh - 1; y >= minh; --y) {
                        mBlock.Set((y << 4) + base, Blocks::WATER);
                        mBrightness[(y << 4) + base] = static_cast<Brightness>(cur_br);
                        cur_br -= 2;
                        if (cur_br < BRIGHTNESSMIN) cur_br = BRIGHTNESSMIN;
                    }
                }
                //Rock layer
                maxh = std::min(std::max(0, h - 5), 16);
                for (auto y = 0; y < maxh; ++y) mBlock.Set((y << 4) + base, Blocks::ROCK);
                //Air layer
                for (auto y = std::min(std::max(0, std::max(h + 1, wh + 1)), 16); y < 16; ++y) {
                    mBlock.Set((y << 4) + base, Blocks::ENV);
                    mBrightness[(y << 4) + base] = skylight;
                }
                //Bedrock layer (overwrite)
                if (cy == 0) mBlock.Set(base, Blocks::BEDROCK);
            }
        }
    }

    void Chunk::buildDetail() {
        auto index = 0;
        for (auto x = 0; x < 16; x++) {
            for (auto y = 0; y < 16; y++) {
                for (auto z = 0; z < 16; z++) {
                    //Tree
                    if (mBlock.Get(index) == Blocks::GRASS && rnd() < 0.005) {
                        buildtree(cx * 16 + x, cy * 16 + y, cz * 16 + z);
                    }
                    index++;
                }
            }
        }
    }

    void Chunk::build(bool initIfEmpty) {
        buildTerrain(initIfEmpty);
        if (!Empty) buildDetail();
    }

    void Chunk::Load(bool initIfEmpty) {
        //assert(Empty == false);

        create();
#ifndef NEWORLD_DEBUG_NO_FILEIO
        if (!LoadFromFile()) build(initIfEmpty);
#else
        build(initIfEmpty);
#endif
        if (!Empty) updated = true;
    }

    Chunk::~Chunk() {
        unloadedChunksCount++;
        SaveToFile();
        destroyRender();
        destroy();
    }

    bool Chunk::LoadFromFile() {
        // TODO(no implementation)
        return false;
    }

    void Chunk::SaveToFile() {
        if (!Empty && Modified) {
            // TODO(no implementation)
        }
        if (!objects.empty()) {

        }
    }

    void Chunk::buildRender() {
        for (auto x = -1; x <= 1; x++) {
            for (auto y = -1; y <= 1; y++) {
                for (auto z = -1; z <= 1; z++) {
                    if (x == 0 && y == 0 && z == 0) continue;
                    if (ChunkOutOfBound({(cx + x), (cy + y), (cz + z)})) continue;
                    if (!ChunkLoaded({(cx + x), (cy + y), (cz + z)})) return;
                }
            }
        }

        rebuiltChunks++;
        updatedChunks++;

        if (!renderBuilt) {
            renderBuilt = true;
            loadAnim = cy * 16.0f + 16.0f;
        }

        if (MergeFace) ChunkRenderer::MergeFaceRender(this);
        else ChunkRenderer::RenderChunk(this);
        if (Renderer::AdvancedRender) ChunkRenderer::RenderDepthModel(this);

        updated = false;

    }

    void Chunk::destroyRender() {
        if (!renderBuilt) return;
        if (vbuffer[0] != 0) vbuffersShouldDelete.push_back(vbuffer[0]);
        if (vbuffer[1] != 0) vbuffersShouldDelete.push_back(vbuffer[1]);
        if (vbuffer[2] != 0) vbuffersShouldDelete.push_back(vbuffer[2]);
        if (vbuffer[3] != 0) vbuffersShouldDelete.push_back(vbuffer[3]);
        vbuffer[0] = vbuffer[1] = vbuffer[2] = vbuffer[3] = 0;
        renderBuilt = false;
    }

    Hitbox::AABB Chunk::getBaseAABB() {
        Hitbox::AABB ret{};
        ret.xmin = cx * 16 - 0.5;
        ret.ymin = cy * 16 - 0.5;
        ret.zmin = cz * 16 - 0.5;
        ret.xmax = cx * 16 + 16 - 0.5;
        ret.ymax = cy * 16 + 16 - 0.5;
        ret.zmax = cz * 16 + 16 - 0.5;
        return ret;
    }

    Frustum::ChunkBox Chunk::getRelativeAABB() {
        Frustum::ChunkBox ret{};
        ret.xmin = static_cast<float>(aabb.xmin - relBaseX);
        ret.xmax = static_cast<float>(aabb.xmax - relBaseX);
        ret.ymin = static_cast<float>(aabb.ymin - loadAnim - relBaseY);
        ret.ymax = static_cast<float>(aabb.ymax - loadAnim - relBaseY);
        ret.zmin = static_cast<float>(aabb.zmin - relBaseZ);
        ret.zmax = static_cast<float>(aabb.zmax - relBaseZ);
        return ret;
    }

}
