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
private:

public:
    // Setup rendering
    void init();
    // Reset translations/rotations (Restore transform matrixes)
    void restoreScale()
    {
        glLoadIdentity();
    }
    // Apply translations
    void translate(const Vec3f& delta)
    {
        glTranslatef(delta.x, delta.y, delta.z);
    }
    // Apply rotations
    void rotate(float degrees, const Vec3f& scale)
    {
        glRotatef(degrees, scale.x, scale.y, scale.z);
    }
    // Perspective projection
    void setPerspective(float fov, float aspect, float zNear, float zFar)
    {
        glMultMatrixf(Mat4f::perspective(fov, aspect, zNear, zFar).getTranspose().data);
    }
    // Orthogonal projection
    void setOrtho(float left, float right, float top, float bottom, float zNear, float zFar)
    {
        glMultMatrixf(Mat4f::ortho(left, right, top, bottom, zNear, zFar).getTranspose().data);
    }

};

#endif // !RENDERER_H_
