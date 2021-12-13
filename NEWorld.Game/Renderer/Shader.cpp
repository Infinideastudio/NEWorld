#include <cstring>
#include <fstream>
#include <iostream>
#include "Shader.h"
#include "FunctionsKit.h"
#include "Temp/String.h"

static temp::string LoadFile(const std::string& path) {
    auto ss = temp::ostringstream{};
    std::ifstream input_file(path);
    if (!input_file.is_open()) {
        throw std::runtime_error("No such file:" + path);
    }
    ss << input_file.rdbuf();
    return ss.str();
}

Shader::Shader(std::string vshPath, std::string fshPath, bool bindLocation, std::set<std::string> defines) {
    const auto sourceVsh = LoadFile(vshPath);
    const auto sourceFsh = LoadFile(fshPath);
    std::map<int, std::string> binds{};
    if (bindLocation) binds[1] = "VertexAttrib";
    mProgram = Renderer::CreateProgram(
        Renderer::Compile(Renderer::ShaderType::Vertex, sourceVsh, {}),
        Renderer::Compile(Renderer::ShaderType::Fragment, sourceFsh, {}),
        binds
    );
}

void Shader::release() {
    glDeleteProgram(mProgram);
}

bool Shader::setUniform(const char *uniform, float value) {
    const auto loc = glGetUniformLocation(mProgram, uniform);
    assert(loc != -1);
    if (loc == -1) return false;
    glUniform1fARB(loc, value);
    return true;
}

bool Shader::setUniform(const char *uniform, int value) {
    const auto loc = glGetUniformLocation(mProgram, uniform);
    assert(loc != -1);
    if (loc == -1) return false;
    glUniform1iARB(loc, value);
    return true;
}

bool Shader::setUniform(const char *uniform, float v0, float v1, float v2, float v3) {
    const auto loc = glGetUniformLocation(mProgram, uniform);
    assert(loc != -1);
    if (loc == -1) return false;
    glUniform4fARB(loc, v0, v1, v2, v3);
    return true;
}

bool Shader::setUniform(const char *uniform, float *value) {
    const auto loc = glGetUniformLocation(mProgram, uniform);
    assert(loc != -1);
    if (loc == -1) return false;
    glUniformMatrix4fvARB(loc, 1, GL_FALSE, value);
    return true;
}
