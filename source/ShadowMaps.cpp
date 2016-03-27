#include "stdinclude.h"
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