#pragma once

#include <set>
#include <string>
#include "stdinclude.h"
#include "Renderer/GL/Pipeline.h"

class Shader {
public:
    Shader(std::string vshPath, std::string fshPath, bool bindLocation = false) : Shader(vshPath, fshPath, bindLocation,
                                                                                         std::set<std::string>()) {}

    Shader(std::string vshPath, std::string fshPath, bool bindLocation, std::set<std::string> defines);

    void bind() { glUseProgram(mProgram); }

    static void unbind() { glUseProgram(0); }

    void release();

    bool setUniform(const char *uniform, float value);

    bool setUniform(const char *uniform, int value);

    bool setUniform(const char *uniform, float v0, float v1, float v2, float v3);

    bool setUniform(const char *uniform, float *value);

private:
    GLuint mProgram;
};
