#include "Definitions.h"
#include "Setup.h"
#include "TextRenderer.h"
#include "Renderer.h"

void SetupScreen()
{
    //Get OpenGL Version
    gl_version_major = glfwGetWindowAttrib(MainWindow, GLFW_CONTEXT_VERSION_MAJOR);
    gl_version_minor = glfwGetWindowAttrib(MainWindow, GLFW_CONTEXT_VERSION_MINOR);
    gl_version_rev = glfwGetWindowAttrib(MainWindow, GLFW_CONTEXT_REVISION);
#ifndef NEWORLD_TARGET_MACOSX
    //Get OpenGL Procedure Addresses
    InitGLProc();
#endif
    //Config Rendering Arguments
    glViewport(0, 0, windowwidth, windowheight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glShadeModel(GL_SMOOTH);
    glDisable(GL_DITHER);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
    glDepthFunc(GL_LEQUAL);
    glAlphaFunc(GL_GREATER, 0.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glHint(GL_FOG_HINT, GL_FASTEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
    if (Multisample)
        glEnable(GL_MULTISAMPLE_ARB);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glColor4f(0.0, 0.0, 0.0, 1.0);
    TextRenderer::BuildFont(windowwidth, windowheight);
    TextRenderer::setFontColor(1.0, 1.0, 1.0, 1.0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearDepth(1.0);
    if (Renderer::AdvancedRender)
        Renderer::InitShaders();

    glfwSwapInterval(vsync);
}

void SetupNormalFog()
{
    float fogColor[4] = { skycolorR, skycolorG, skycolorB, 1.0f };
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_START, ViewDistance * 16.0f - 32.0f);
    glFogf(GL_FOG_END, ViewDistance * 16.0f);
}
