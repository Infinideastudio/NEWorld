#pragma once
#include "StdInclude.h"
#include "World.h"
#include "Renderer.h"

namespace WorldRenderer
{
struct RenderChunk
{
    int cx, cy, cz;
    vtxCount vertexes[4];
    VBOID vbuffers[4];
    double loadAnim;
    RenderChunk(World::chunk* c, double TimeDelta) :
        cx(c->cx), cy(c->cy), cz(c->cz), loadAnim(c->loadAnim * pow(0.6, TimeDelta))
    {
        memcpy(vbuffers, c->vbuffer, sizeof(vbuffers));
        memcpy(vertexes, c->vertexes, sizeof(vertexes));
    }
};
extern vector<RenderChunk> RenderChunkList;

int ListRenderChunks(int cx, int cy, int cz, int renderdistance, double curtime, bool frustest = true);
void RenderChunks(double x, double y, double z, int buffer);
}