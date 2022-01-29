#include "Chunk.h"
#include "World.h"

namespace World {
    Chunk::~Chunk() {
        unloadedChunksCount++;
    }

    BoundingBox Chunk::getBaseAABB() {
        const auto min = Double3(GetPosition() * 16) - Double3(0.5);
        const auto max = Double3(GetPosition() * 16) + Double3(16.0 - 0.5);
        return BoundingBox{toBvhVec(min), toBvhVec(max)};
    }
}
