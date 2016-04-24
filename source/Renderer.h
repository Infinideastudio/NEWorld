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

#ifndef RENDERER_H
#define RENDERER_H

#include "Definitions.h"
#include "GLProc.h"
#include "Frustum.h"
#include "Shader.h"

namespace Renderer
{
enum
{
    MainShader, MergeFaceShader, ShadowShader, DepthShader
};

const int ArraySize = 2621440;
extern float* VA;
extern int size;
extern int Vertexes;
extern bool AdvancedRender;
extern int ShadowRes;
extern int MaxShadowDist;
extern int shadowdist;
extern float sunlightXrot, sunlightYrot;
extern unsigned int DepthTexture;
extern vector<Shader> shaders;
extern int ActiveShader;

void Init(int tc, int cc, int ac = 0);
void Vertex3f(float x, float y, float z);
void TexCoord2f(float x, float y);
void TexCoord3f(float x, float y, float z);
void Color3f(float r, float g, float b);
void Color4f(float r, float g, float b, float a);
void Attrib1f(float attr);

inline void Vertex3d(double x, double y, double z)
{
    Vertex3f((float)x, (float)y, (float)z);
}

inline void TexCoord2d(double x, double y)
{
    TexCoord2f((float)x, (float)y);
}

inline void TexCoord3d(double x, double y, double z)
{
    TexCoord3f((float)x, (float)y, (float)z);
}

inline void Color3d(double r, double g, double b)
{
    Color3f((float)r, (float)g, (float)b);
}

inline void Color4d(double r, double g, double b, double a)
{
    Color4f((float)r, (float)g, (float)b, (float)a);
}

inline void Quad(float *geomentry)
{
    memcpy(VA, geomentry, size*sizeof(float));
    VA += size;
    Vertexes += 4;
}

void Flush(VBOID& buffer, vtxCount& vtxs);
void renderbuffer(VBOID buffer, vtxCount vtxs, int tc, int cc, int ac = 0);

void InitShaders();
inline void bindShader(int shaderID)
{
    shaders[shaderID].bind();
    ActiveShader = shaderID;
}
void DestroyShaders();
void EnableShaders();
void DisableShaders();
void StartShadowPass();
void EndShadowPass();
}

#endif
