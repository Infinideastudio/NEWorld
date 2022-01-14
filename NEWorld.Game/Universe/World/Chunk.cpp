#include "Chunk.h"
#include "World.h"

namespace World {
    double Chunk::relBaseX, Chunk::relBaseY, Chunk::relBaseZ;

    Chunk::~Chunk() {
        unloadedChunksCount++;
    }

    BoundingBox Chunk::getBaseAABB() {
        const auto min = Double3(GetPosition() * 16) - Double3(0.5);
        const auto max = Double3(GetPosition() * 16) + Double3(16.0 - 0.5);
        return BoundingBox{toBvhVec(min), toBvhVec(max)};
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
