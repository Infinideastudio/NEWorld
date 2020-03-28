// 
// GUI: renderer.h
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#pragma once

#include "opengl.h"
#include "Common/Math/Matrix.h"
#include "Common/Logger.h"
#include <Math/Vector3.h>

class Renderer {
public:
    // Setup rendering
    static void init();

    static void setViewport(int x, int y, int width, int height) { glViewport(x, y, width, height); }

    // Reset translations/rotations (Restore transform matrixes)
    static void restoreScale() { mModel = Mat4f{1.0f}; }

    // Apply translation
    static void translate(const Float3& delta) {
        mModel *= Mat4f::translation(delta);
    }

    // Apply rotation
    static void rotate(float degrees, const Float3& scale) {
        mModel *= Mat4f::rotation(degrees, scale);
    }

    // Reset translations/rotations (Restore transform matrixes)
    static void restoreView() { mView = Mat4f{1.0f}; }

    // Apply translation
    static void translateView(const Float3& delta) {
        mView *= Mat4f::translation(delta);
    }

    // Apply rotation
    static void rotateView(float degrees, const Float3& scale) {
        mView *= Mat4f::rotation(degrees, scale);
    }

    // Restore projection matrix
    static void restoreProj() { mProjection = Mat4f{1.0f}; }

    // Perspective projection
    static void applyPerspective(float fov, float aspect, float zNear, float zFar) {
        mProjection *= Mat4f::perspective(fov, aspect, zNear, zFar);
    }

    // Orthogonal projection
    static void applyOrtho(float left, float right, float top, float bottom, float zNear, float zFar) {
        mProjection *= Mat4f::ortho(left, right, top, bottom, zNear, zFar);
    }

    static void clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

    static void flush() { glFlush(); }

    static void waitForComplete() { glFinish(); }

    static void checkError() {
        GLenum err = glGetError();
        if (err) warningstream << "OpenGL Error " << err;
    }

    static Mat4f getMvpMatrix() noexcept { return mProjection * mView * mModel; }

    static void configShader(const std::string& name);

    static void startFrame();

    static void endFrame();

    static void setMatrix();
private:
    inline static Mat4f mModel{}, mProjection{}, mView{};
};
