#include "WorldRenderer.h"

namespace WorldRenderer
{
vector<RenderChunk> RenderChunkList;

int ListRenderChunks(int cx, int cy, int cz, int renderdistance, double curtime, bool frustest)
{
    int renderedChunks = 0;
    RenderChunkList.clear();
    for (int i = 0; i < World::loadedChunks; i++)
    {
        if (!World::chunks[i]->renderBuilt || World::chunks[i]->Empty) continue;
        if (World::chunkInRange(World::chunks[i]->cx, World::chunks[i]->cy, World::chunks[i]->cz,
                                cx, cy, cz, renderdistance))
        {
            if (!frustest || World::chunks[i]->visible)
            {
                renderedChunks++;
                RenderChunkList.push_back(RenderChunk(World::chunks[i], (curtime - lastupdate) * 30.0));
            }
        }
    }
    return renderedChunks;
}

void RenderChunks(double x, double y, double z, int buffer)
{
    int TexcoordCount = MergeFace ? 3 : 2, ColorCount = 3;
    float m[16];
    if (buffer != 3)
    {
        memset(m, 0, sizeof(m));
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }
    else TexcoordCount = ColorCount = 0;

    for (unsigned int i = 0; i < RenderChunkList.size(); i++)
    {
        RenderChunk cr = RenderChunkList[i];
        if (cr.vertexes[0] == 0) continue;
        glPushMatrix();
        glTranslated(cr.cx * 16.0 - x, cr.cy * 16.0 - cr.loadAnim - y, cr.cz * 16.0 - z);
        if (Renderer::AdvancedRender && buffer != 3)
        {
            m[12] = cr.cx * 16.0f - (float)x;
            m[13] = cr.cy * 16.0f - (float)cr.loadAnim - (float)y;
            m[14] = cr.cz * 16.0f - (float)z;
            Renderer::shaders[Renderer::ActiveShader].setUniform("TransMat", m);
            Renderer::renderbuffer(cr.vbuffers[buffer], cr.vertexes[buffer], TexcoordCount, ColorCount, 1);
        }
        else Renderer::renderbuffer(cr.vbuffers[buffer], cr.vertexes[buffer], TexcoordCount, ColorCount);
        glPopMatrix();
    }

    glFlush();
}
}