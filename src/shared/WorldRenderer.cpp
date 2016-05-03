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

#include "WorldRenderer.h"

namespace WorldRenderer
{
vector<RenderChunk> RenderChunkList;

size_t ListRenderChunks(int cx, int cy, int cz, int renderdistance, double curtime, bool frustest)
{
    size_t renderedChunks = 0u;
    RenderChunkList.clear();
    for (size_t i = 0; i < World::chunks.size(); i++)
    {
        if (World::chunks[i]->renderBuilt && !World::chunks[i]->Empty && World::chunkInRange(World::chunks[i]->cx, World::chunks[i]->cy, World::chunks[i]->cz,
                cx, cy, cz, renderdistance) && (!frustest || World::chunks[i]->visible))
        {
            renderedChunks++;
            RenderChunkList.push_back(RenderChunk(World::chunks[i], (curtime - lastupdate) * 30.0));
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