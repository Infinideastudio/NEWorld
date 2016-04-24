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

#ifndef SHADER_H
#define SHADER_H

#include "Definitions.h"
#include "GLProc.h"

class Shader
{
public:
    Shader(string vshPath, string fshPath, bool bindLocation = false) :Shader(vshPath, fshPath, bindLocation, std::set<string>()) {}
    Shader(string vshPath, string fshPath, bool bindLocation, std::set<string> defines);

    void bind()
    {
        glUseProgramObjectARB(shaderProgram);
    }

    static void unbind()
    {
        glUseProgramObjectARB(0);
    }

    void release();

    bool setUniform(const char* uniform, float value);
    bool setUniform(const char* uniform, int value);
    bool setUniform(const char* uniform, float v0, float v1, float v2, float v3);
    bool setUniform(const char* uniform, float* value);

private:
    GLhandleARB loadShader(string filename, unsigned int mode, std::set<string> defines);
    void checkErrors(GLhandleARB res, int status, string errorMessage);

    GLhandleARB shaderFragment;
    GLhandleARB shaderVertex;
    GLhandleARB shaderProgram;
};

#endif
