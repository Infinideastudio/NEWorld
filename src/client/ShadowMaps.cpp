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

#include "Definitions.h"
#include "ShadowMaps.h"

namespace ShadowMaps
{
void BuildShadowMap(double xpos, double ypos, double zpos, double curtime)
{
    int cx = getchunkpos((int)xpos), cy = getchunkpos((int)ypos), cz = getchunkpos((int)zpos);

    Renderer::StartShadowPass();
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);
    glDisable(GL_BLEND);
    float scale = 16.0f * sqrt(3.0f);
    float length = Renderer::shadowdist*scale;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-length, length, -length, length, -length, length);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotated(Renderer::sunlightXrot, 1.0, 0.0, 0.0);
    glRotated(Renderer::sunlightYrot, 0.0, 1.0, 0.0);

    WorldRenderer::ListRenderChunks(cx, cy, cz, Renderer::shadowdist + 1, curtime, false);
    MutexUnlock(Mutex);
    WorldRenderer::RenderChunks(xpos, ypos, zpos, 3);
    MutexLock(Mutex);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    Renderer::EndShadowPass();

    glEnable(GL_FOG);
    glEnable(GL_BLEND);
}

void RenderShadowMap(double xpos, double ypos, double zpos, double curtime)
{
    int cx = getchunkpos((int)xpos), cy = getchunkpos((int)ypos), cz = getchunkpos((int)zpos);

    Renderer::bindShader(Renderer::DepthShader);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);
    glDisable(GL_BLEND);
    float scale = 16.0f * sqrt(3.0f);
    float length = Renderer::shadowdist*scale;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-length, length, -length, length, -length, length);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotated(Renderer::sunlightXrot, 1.0, 0.0, 0.0);
    glRotated(Renderer::sunlightYrot, 0.0, 1.0, 0.0);

    WorldRenderer::ListRenderChunks(cx, cy, cz, Renderer::shadowdist + 1, curtime, false);
    MutexUnlock(Mutex);
    WorldRenderer::RenderChunks(xpos, ypos, zpos, 3);
    MutexLock(Mutex);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    Shader::unbind();

    glEnable(GL_FOG);
    glEnable(GL_BLEND);
}
}