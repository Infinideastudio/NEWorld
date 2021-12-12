#pragma once

#include "Universe/World/World.h"
#include "Renderer.h"

namespace WorldRenderer {
    struct RenderChunk {
        Int3 position;
        vtxCount vertexes[4];
        VBOID vbuffers[4];
        double loadAnim;

        RenderChunk(World::Chunk *c, double TimeDelta) :
                position(c->GetPosition()), loadAnim(c->loadAnim * pow(0.6, TimeDelta)) {
            memcpy(vbuffers, c->vbuffer, sizeof(vbuffers));
            memcpy(vertexes, c->vertexes, sizeof(vertexes));
        }
    };

    extern std::vector<RenderChunk> RenderChunkList;

    int ListRenderChunks(int cx, int cy, int cz, int renderdistance, double curtime, bool frustest = true);

    void RenderChunks(double x, double y, double z, int buffer);
}