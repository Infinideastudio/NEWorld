/*
* NEWorld: A free game with similar rules to Minecraft.
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

#ifndef RENDERER_H_
#define RENDERER_H_

#include "opengl.h"
#include <vec3.h>
#include "mat4.h"
#include "vertexarray.h"

class Renderer
{
public:
    // Setup rendering
    static void init(int width, int height);

    static void setViewport(int x, int y, int width, int height)
    {
        glViewport(x, y, width, height);
    }

    // Reset translations/rotations (Restore transform matrixes)
    static void restoreScale()
    {
        setModelMatrix();
        glLoadIdentity();
    }

    // Apply translations
    static void translate(const Vec3f& delta)
    {
        setModelMatrix();
        glTranslatef(delta.x, delta.y, delta.z);
    }

    // Apply rotations
    static void rotate(float degrees, const Vec3f& scale)
    {
        setModelMatrix();
        glRotatef(degrees, scale.x, scale.y, scale.z);
    }

    // Restore projection matrix
    static void restoreProj()
    {
        setProjMatrix();
        glLoadIdentity();
    }

    // Perspective projection
    static void applyPerspective(float fov, float aspect, float zNear, float zFar)
    {
        setProjMatrix();
        glMultMatrixf(Mat4f::perspective(fov, aspect, zNear, zFar).getTranspose().data);
    }

    // Orthogonal projection
    static void applyOrtho(float left, float right, float top, float bottom, float zNear, float zFar)
    {
        setProjMatrix();
        glMultMatrixf(Mat4f::ortho(left, right, top, bottom, zNear, zFar).getTranspose().data);
    }

    static void clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

private:
    static int matrixMode;

    static void setProjMatrix()
    {
        if (matrixMode == 1) return;
        glMatrixMode(GL_PROJECTION);
        matrixMode = 1;
    }

    static void setModelMatrix()
    {
        if (matrixMode == 0) return;
        glMatrixMode(GL_MODELVIEW);
        matrixMode = 0;
    }
};

#endif // !RENDERER_H_
