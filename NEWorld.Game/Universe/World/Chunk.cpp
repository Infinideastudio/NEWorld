#include "Chunk.h"
#include "World.h"

namespace ChunkRenderer {
    void RenderChunk(World::Chunk *c);

    void RenderDepthModel(World::Chunk *c);
}

namespace Renderer {
    extern bool AdvancedRender;
}

namespace World {
    double Chunk::relBaseX, Chunk::relBaseY, Chunk::relBaseZ;
    Frustum Chunk::TestFrustum;

    Chunk::~Chunk() {
        unloadedChunksCount++;
        destroyRender();
    }

    void Chunk::buildRender() {
        for (auto x = -1; x <= 1; x++) {
            for (auto y = -1; y <= 1; y++) {
                for (auto z = -1; z <= 1; z++) {
                    if (x == 0 && y == 0 && z == 0) continue;
                    if (ChunkOutOfBound(GetPosition() + Int3{x, y, z})) continue;
                    if (!ChunkLoaded(GetPosition() + Int3{x, y, z})) return;
                }
            }
        }

        rebuiltChunks++;
        updatedChunks++;

        if (!renderBuilt) {
            renderBuilt = true;
            loadAnim = static_cast<float>(GetPosition().Y) * 16.0f + 16.0f;
        }

        ChunkRenderer::RenderChunk(this);
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

    BoundingBox Chunk::getBaseAABB() {
        const auto min = Double3(GetPosition() * 16) - Double3(0.5);
        const auto max = Double3(GetPosition() * 16) + Double3(16.0 - 0.5);
        return BoundingBox{min, max};
    }

    Frustum::ChunkBox Chunk::getRelativeAABB() {
        Frustum::ChunkBox ret{};
        ret.xmin = static_cast<float>(mBounds.min.values[0] - relBaseX);
        ret.xmax = static_cast<float>(mBounds.max.values[0] - relBaseX);
        ret.ymin = static_cast<float>(mBounds.min.values[1] - loadAnim - relBaseY);
        ret.ymax = static_cast<float>(mBounds.max.values[1] - loadAnim - relBaseY);
        ret.zmin = static_cast<float>(mBounds.min.values[2] - relBaseZ);
        ret.zmax = static_cast<float>(mBounds.max.values[2] - relBaseZ);
        return ret;
    }
}
