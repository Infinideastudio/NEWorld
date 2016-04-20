#include "Setup.h"
#include "GUI.h"
#include "Definitions.h"
#include "Textures.h"
#include "TextRenderer.h"
#include "Renderer.h"
#include "World.h"
#include "Items.h"

void SplashScreen()
{
    TextureID splTex = Textures::LoadRGBTexture("Textures/GUI/splashscreen.bmp");
    glEnable(GL_TEXTURE_2D);
    for (int i = 0; i < 256; i += 2)
    {
        glfwSwapBuffers(MainWindow);
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, splTex);
        glColor4f((float)i / 256, (float)i / 256, (float)i / 256, 1.0);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 1.0);
        glVertex2i(-1, 1);
        glTexCoord2f(850.0f / 1024.0f, 1.0);
        glVertex2i(1, 1);
        glTexCoord2f(850.0f / 1024.0f, 1.0 - 480.0f / 1024.0f);
        glVertex2i(1, -1);
        glTexCoord2f(0.0, 1.0 - 480.0f / 1024.0f);
        glVertex2i(-1, -1);
        glEnd();
    }
    glDeleteTextures(1, &splTex);
    glfwSwapBuffers(MainWindow);
    glfwPollEvents();
}

void createWindow()
{
    glfwSetErrorCallback([](int, const char* desc)
    {
        cout << "We are sorry to inform you that NEWorld has crashed." << endl
             << "It is probably caused by a badly-written plugin or a bug." << endl
             << "You can post an issue on the GitHub repository at" << endl
             << "https://github.com/Infinideastudio/NEWorld/issues" << endl
             << "You're welcomed to fix the bug and post a PR." << endl
             << "==========" << endl
             << "Technical Information:" << endl
             << "Reason:" << endl << desc << endl
             << "Logs:" << endl;
        cout << GlobalLogger.ExportAll() << endl;
    });
    if (Multisample)
        glfwWindowHint(GLFW_SAMPLES, Multisample);
    MainWindow = glfwCreateWindow(windowwidth, windowheight, ("NEWorld " + major_version + minor_version + ext_version).c_str(), NULL, NULL);
    MouseCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    glfwMakeContextCurrent(MainWindow);
    glfwSetCursor(MainWindow, MouseCursor);
    glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetWindowSizeCallback(MainWindow, [](GLFWwindow * win, int width, int height)
    {
        windowwidth = max(width, 640);
        windowheight = max(height, 360);
        glfwSetWindowSize(win, windowwidth, windowheight);
        SetupScreen();
    });
    glfwSetMouseButtonCallback(MainWindow, [](GLFWwindow *, int button, int action, int)
    {
        mb = 0;
        if (action == GLFW_PRESS)
        {
            if (button == GLFW_MOUSE_BUTTON_LEFT)
                mb = 1;
            if (button == GLFW_MOUSE_BUTTON_RIGHT)
                mb = 2;
            if (button == GLFW_MOUSE_BUTTON_MIDDLE)
                mb = 4;
        }
    });
    glfwSetScrollCallback(MainWindow, [](GLFWwindow *, double, double yoffset)
    {
        mw += (int)yoffset;
    });
    glfwSetCharCallback(MainWindow, [](GLFWwindow *, unsigned int c)
    {
        if (c >= 128)
        {
            wchar_t pwszUnicode[2] = { (wchar_t)c,'\0' };
            char pszMultiByte[5];
            WCharToMByte(pszMultiByte, pwszUnicode, 4);
            inputstr += pszMultiByte;
        }
        else
            inputstr += (char)c;
    });
    if (ppistretch) GUI::InitStretch();
}

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
    glGenBuffersARB(1, &World::EmptyBuffer);
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

void LoadTextures()
{
    tex_select = Textures::LoadRGBATexture("Textures/GUI/select.bmp", "");
    tex_unselect = Textures::LoadRGBATexture("Textures/GUI/unselect.bmp", "");
    tex_title = Textures::LoadRGBATexture("Textures/GUI/title.bmp", "Textures/GUI/titlemask.bmp");
    for (int i = 0; i < 6; i++)
    {
        std::stringstream ss;
        ss << "Textures/GUI/mainmenu" << i << ".bmp";
        tex_mainmenu[i] = Textures::LoadRGBTexture(ss.str());
    }

    DefaultSkin = Textures::LoadRGBATexture("Textures/Player/skin_xiaoqiao.bmp", "Textures/Player/skinmask_xiaoqiao.bmp");

    for (int gloop = 1; gloop <= 10; gloop++)
    {
        string path = "Textures/blocks/destroy_" + pack(gloop) + ".bmp";
        DestroyImage[gloop] = Textures::LoadRGBATexture(path, path);
    }

    BlockTextures = Textures::LoadRGBATexture("Textures/blocks/Terrain.bmp", "Textures/blocks/Terrainmask.bmp");
    BlockTextures3D = Textures::LoadBlock3DTexture("Textures/blocks/Terrain3D.bmp", "Textures/blocks/Terrain3Dmask.bmp");
    LoadItemsTextures();
}
