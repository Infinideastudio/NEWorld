#include "Shader.h"
#include "FunctionsKit.h"
#include "GLProc.h"
Shader::Shader(string vshPath, string fshPath, bool bindLocation, std::set<string> defines)
{
    shaderVertex = loadShader(vshPath, GL_VERTEX_SHADER_ARB, defines);
    shaderFragment = loadShader(fshPath, GL_FRAGMENT_SHADER_ARB, defines);
    shaderProgram = glCreateProgramObjectARB();
    glAttachObjectARB(shaderProgram, shaderVertex);
    glAttachObjectARB(shaderProgram, shaderFragment);
    if (bindLocation)
    {
        glBindAttribLocationARB(shaderProgram, 1, "VertexAttrib");
    }
    glLinkProgramARB(shaderProgram);

    //检查错误
    checkErrors(shaderProgram, GL_LINK_STATUS, "Shader linking error!");
}

void Shader::release()
{
    glDetachObjectARB(shaderProgram, shaderVertex);
    glDetachObjectARB(shaderProgram, shaderFragment);
    glDeleteObjectARB(shaderVertex);
    glDeleteObjectARB(shaderFragment);
    glDeleteObjectARB(shaderProgram);
}

bool Shader::setUniform(const char* uniform, float value)
{
    int loc = glGetUniformLocationARB(shaderProgram, uniform);
    assert(loc != -1);
    if (loc == -1) return false;
    glUniform1fARB(loc, value);
    return true;
}
bool Shader::setUniform(const char* uniform, int value)
{
    int loc = glGetUniformLocationARB(shaderProgram, uniform);
    assert(loc != -1);
    if (loc == -1) return false;
    glUniform1iARB(loc, value);
    return true;
}
bool Shader::setUniform(const char* uniform, float v0, float v1, float v2, float v3)
{
    int loc = glGetUniformLocationARB(shaderProgram, uniform);
    assert(loc != -1);
    if (loc == -1) return false;
    glUniform4fARB(loc, v0, v1, v2, v3);
    return true;
}
bool Shader::setUniform(const char* uniform, float * value)
{
    int loc = glGetUniformLocationARB(shaderProgram, uniform);
    assert(loc != -1);
    if (loc == -1) return false;
    glUniformMatrix4fvARB(loc, 1, GL_FALSE, value);
    return true;
}

GLhandleARB Shader::loadShader(string filename, unsigned int mode, std::set<string> defines)
{
    std::stringstream ss;
    GLhandleARB res;
    string cur, var, macro;
    int lines = 0, curlen;
    char* curline;
    std::vector<char*> source;
    std::vector<int> length;
    std::ifstream filein(filename);
    if (!filein.is_open()) return 0;
    while (!filein.eof())
    {
        std::getline(filein, cur);
        if (cur.empty()) continue;
        if (beginWith(cur, "#"))   //处理NEWorld预处理器标志
        {
            ss.str(cur);
            ss >> macro;
            if (macro == "##NEWORLD_SHADER_DEFINES")
            {
                ss >> var >> macro;
                if (defines.find(var) != defines.end()) cur = "#define " + macro;
                else continue;
            }
        }
        cur += '\n';
        curlen = cur.size();
        curline = new char[curlen];
        memcpy(curline, cur.c_str(), curlen);
        lines++;
        source.push_back(curline);
        length.push_back(curlen);
    }
    filein.close();

    //创建shader
    res = glCreateShaderObjectARB(mode);
    glShaderSourceARB(res, lines, (const GLchar**)source.data(), length.data());
    glCompileShaderARB(res);
    //释放内存
    for (int i = 0; i < lines; i++) delete[] source[i];

    //检查错误
    checkErrors(res, GL_COMPILE_STATUS, "Shader compilation error! File: " + filename);
    return res;
}

void Shader::checkErrors(GLhandleARB res, int status, string errorMessage)
{
    int st = GL_TRUE;
    glGetObjectParameterivARB(res, status, &st);
    if (st == GL_FALSE) DebugWarning(errorMessage);
    int infologLength, charsWritten;
    char* infoLog;
    glGetObjectParameterivARB(res, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);
    if (infologLength > 1)
    {
        infoLog = new char[infologLength];
        glGetInfoLogARB(res, infologLength, &charsWritten, infoLog);
        cout << infoLog << endl;
        delete[] infoLog;
    }
}
