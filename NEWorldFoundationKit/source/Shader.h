#pragma once
#include "StdInclude.h"
#include "GLProc.h"

class Shader
{
public:
    Shader(string vshPath, string fshPath, bool bindLocation = false) :Shader(vshPath, fshPath, bindLocation, std::set<string>()) {}
    Shader(string vshPath, string fshPath, bool bindLocation, std::set<string> defines);

    inline void bind()
    {
        glUseProgramObjectARB(shaderProgram);
    }
    static inline void unbind()
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
