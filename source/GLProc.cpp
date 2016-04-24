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

#include "GLProc.h"

#ifndef NEWORLD_TARGET_MACOSX
//OpenGL Procedure

PFNGLTEXIMAGE3DPROC glTexImage3D;
PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;

PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArrayARB;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glDisableVertexAttribArrayARB;
PFNGLGETATTRIBLOCATIONARBPROC glGetAttribLocationARB;
PFNGLBINDATTRIBLOCATIONARBPROC glBindAttribLocationARB;
PFNGLVERTEXATTRIBPOINTERARBPROC glVertexAttribPointerARB;

PFNGLGENBUFFERSARBPROC glGenBuffersARB;
PFNGLBINDBUFFERARBPROC glBindBufferARB;
PFNGLBUFFERDATAARBPROC glBufferDataARB;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;

PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glFramebufferTexture3DEXT;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;

PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
PFNGLGETINFOLOGARBPROC glGetInfoLogARB;
PFNGLDETACHOBJECTARBPROC glDetachObjectARB;
PFNGLDELETEOBJECTARBPROC glDeleteObjectARB;

PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;
PFNGLUNIFORM1FARBPROC glUniform1fARB;
PFNGLUNIFORM2FARBPROC glUniform2fARB;
PFNGLUNIFORM3FARBPROC glUniform3fARB;
PFNGLUNIFORM4FARBPROC glUniform4fARB;
PFNGLUNIFORM1IARBPROC glUniform1iARB;
PFNGLUNIFORM2IARBPROC glUniform2iARB;
PFNGLUNIFORM3IARBPROC glUniform3iARB;
PFNGLUNIFORM4IARBPROC glUniform4iARB;
PFNGLUNIFORM1FVARBPROC glUniform1fvARB;
PFNGLUNIFORM2FVARBPROC glUniform2fvARB;
PFNGLUNIFORM3FVARBPROC glUniform3fvARB;
PFNGLUNIFORM4FVARBPROC glUniform4fvARB;
PFNGLUNIFORM1IVARBPROC glUniform1ivARB;
PFNGLUNIFORM2IVARBPROC glUniform2ivARB;
PFNGLUNIFORM3IVARBPROC glUniform3ivARB;
PFNGLUNIFORM4IVARBPROC glUniform4ivARB;
PFNGLUNIFORMMATRIX2FVARBPROC glUniformMatrix2fvARB;
PFNGLUNIFORMMATRIX3FVARBPROC glUniformMatrix3fvARB;
PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fvARB;
PFNGLGETUNIFORMFVARBPROC glGetUniformfvARB;
PFNGLGETUNIFORMIVARBPROC glGetUniformivARB;

//获取OpenGL函数地址
void InitGLProc()
{

    glTexImage3D = (PFNGLTEXIMAGE3DPROC)glfwGetProcAddress("glTexImage3DEXT");
    glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)glfwGetProcAddress("glTexSubImage3DEXT");
    glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)glfwGetProcAddress("glActiveTextureARB");

    glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)glfwGetProcAddress("glEnableVertexAttribArrayARB");
    glDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)glfwGetProcAddress("glDisableVertexAttribArrayARB");
    glGetAttribLocationARB = (PFNGLGETATTRIBLOCATIONARBPROC)glfwGetProcAddress("glGetAttribLocationARB");
    glBindAttribLocationARB = (PFNGLBINDATTRIBLOCATIONARBPROC)glfwGetProcAddress("glBindAttribLocationARB");
    glVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC)glfwGetProcAddress("glVertexAttribPointerARB");

    glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)glfwGetProcAddress("glGenBuffersARB");
    glBindBufferARB = (PFNGLBINDBUFFERARBPROC)glfwGetProcAddress("glBindBufferARB");
    glBufferDataARB = (PFNGLBUFFERDATAARBPROC)glfwGetProcAddress("glBufferDataARB");
    glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)glfwGetProcAddress("glDeleteBuffersARB");

    glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)glfwGetProcAddress("glGenFramebuffersEXT");
    glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)glfwGetProcAddress("glBindFramebufferEXT");
    glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)glfwGetProcAddress("glDeleteFramebuffersEXT");
    glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)glfwGetProcAddress("glFramebufferTexture2DEXT");
    glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)glfwGetProcAddress("glFramebufferTexture3DEXT");
    glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)glfwGetProcAddress("glCheckFramebufferStatusEXT");

    glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)glfwGetProcAddress("glCreateShaderObjectARB");
    glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)glfwGetProcAddress("glShaderSourceARB");
    glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)glfwGetProcAddress("glCompileShaderARB");
    glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)glfwGetProcAddress("glCreateProgramObjectARB");
    glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)glfwGetProcAddress("glAttachObjectARB");
    glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)glfwGetProcAddress("glLinkProgramARB");
    glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)glfwGetProcAddress("glUseProgramObjectARB");
    glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)glfwGetProcAddress("glGetObjectParameterivARB");
    glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)glfwGetProcAddress("glGetInfoLogARB");
    glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)glfwGetProcAddress("glDetachObjectARB");
    glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)glfwGetProcAddress("glDeleteObjectARB");

    glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)glfwGetProcAddress("glGetUniformLocationARB");
    glUniform1fARB = (PFNGLUNIFORM1FARBPROC)glfwGetProcAddress("glUniform1fARB");
    glUniform2fARB = (PFNGLUNIFORM2FARBPROC)glfwGetProcAddress("glUniform2fARB");
    glUniform3fARB = (PFNGLUNIFORM3FARBPROC)glfwGetProcAddress("glUniform3fARB");
    glUniform4fARB = (PFNGLUNIFORM4FARBPROC)glfwGetProcAddress("glUniform4fARB");
    glUniform1iARB = (PFNGLUNIFORM1IARBPROC)glfwGetProcAddress("glUniform1iARB");
    glUniform2iARB = (PFNGLUNIFORM2IARBPROC)glfwGetProcAddress("glUniform2iARB");
    glUniform3iARB = (PFNGLUNIFORM3IARBPROC)glfwGetProcAddress("glUniform3iARB");
    glUniform4iARB = (PFNGLUNIFORM4IARBPROC)glfwGetProcAddress("glUniform4iARB");
    glUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)glfwGetProcAddress("glUniform1fvARB");
    glUniform2fvARB = (PFNGLUNIFORM2FVARBPROC)glfwGetProcAddress("glUniform2fvARB");
    glUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)glfwGetProcAddress("glUniform3fvARB");
    glUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)glfwGetProcAddress("glUniform4fvARB");
    glUniform1ivARB = (PFNGLUNIFORM1IVARBPROC)glfwGetProcAddress("glUniform1ivARB");
    glUniform2ivARB = (PFNGLUNIFORM2IVARBPROC)glfwGetProcAddress("glUniform2ivARB");
    glUniform3ivARB = (PFNGLUNIFORM3IVARBPROC)glfwGetProcAddress("glUniform3ivARB");
    glUniform4ivARB = (PFNGLUNIFORM4IVARBPROC)glfwGetProcAddress("glUniform4ivARB");
    glUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC)glfwGetProcAddress("glUniformMatrix2fvARB");
    glUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC)glfwGetProcAddress("glUniformMatrix3fvARB");
    glUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)glfwGetProcAddress("glUniformMatrix4fvARB");
    glGetUniformfvARB = (PFNGLGETUNIFORMFVARBPROC)glfwGetProcAddress("glGetUniformfvARB");
    glGetUniformivARB = (PFNGLGETUNIFORMIVARBPROC)glfwGetProcAddress("glGetUniformivARB");
}

#endif
