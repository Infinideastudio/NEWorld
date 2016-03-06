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

/*
void DrawShadowMap(int xi, int yi, int xa, int ya) {
	glDisable(GL_TEXTURE_2D);
	glColor4f(0.2f, 0.2f, 0.2f, 0.5f);
	glBegin(GL_QUADS);
	glVertex2i(xi, ya);
	glVertex2i(xa, ya);
	glVertex2i(xa, yi);
	glVertex2i(xi, yi);
	glEnd();

	//glActiveTextureARB(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Renderer::DepthTexture);
	Renderer::bindShader(Renderer::DepthShader);
	//glUniform1iARB(glGetUniformLocationARB(Renderer::shaderPrograms[2], "Tex"), 1);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(xi, ya);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(xa, ya);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(xa, yi);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(xi, yi);
	glEnd();

	//glActiveTextureARB(GL_TEXTURE0);
	Renderer::unbindShader();
}
*/
}