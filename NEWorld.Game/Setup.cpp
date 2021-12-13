#include <iostream>
#include "Setup.h"
#include "Definitions.h"
#include "Textures.h"
#include "TextRenderer.h"
#include "Renderer.h"
#include "Universe/World/World.h"
#include "Items.h"

void splashScreen() {
    auto splTex = Textures::LoadRGBTexture("./Assets/Textures/GUI/SplashScreen.bmp");
    glEnable(GL_TEXTURE_2D);
    for (auto i = 0; i < 256; i += 2) {
        glfwSwapBuffers(MainWindow);
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, splTex);
        glColor4f(static_cast<float>(i) / 256, static_cast<float>(i) / 256, static_cast<float>(i) / 256, 1.0);
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
        SleepMs(10);
    }
    glDeleteTextures(1, &splTex);
    glfwSwapBuffers(MainWindow);
    glfwPollEvents();
}

void createWindow() {
    glfwSetErrorCallback([](int, const char *desc) { std::cout << desc << std::endl; });
    std::stringstream title;
    title << "NEWorld " << MAJOR_VERSION << MINOR_VERSION << EXT_VERSION;
    if (Multisample != 0) glfwWindowHint(GLFW_SAMPLES, Multisample);
    MainWindow = glfwCreateWindow(windowwidth, windowheight, title.str().c_str(), NULL, NULL);

    // high dpi screens deserve a larger window
    float widthScale, heightScale;
    glfwGetWindowContentScale(MainWindow, &widthScale, &heightScale);
    windowwidth *= widthScale;
    windowheight *= heightScale;
    glfwSetWindowSize(MainWindow, windowwidth, windowheight);

    MouseCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    glfwMakeContextCurrent(MainWindow);
    glewInit();
    glfwSetCursor(MainWindow, MouseCursor);
    glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetWindowSizeCallback(MainWindow, &WindowSizeFunc);
    glfwSetMouseButtonCallback(MainWindow, &MouseButtonFunc);
    glfwSetScrollCallback(MainWindow, &MouseScrollFunc);
    glfwSetCharCallback(MainWindow, &CharInputFunc);
}

void setupScreen() {

    //获取OpenGL版本
    GLVersionMajor = glfwGetWindowAttrib(MainWindow, GLFW_CONTEXT_VERSION_MAJOR);
    GLVersionMinor = glfwGetWindowAttrib(MainWindow, GLFW_CONTEXT_VERSION_MINOR);
    GLVersionRev = glfwGetWindowAttrib(MainWindow, GLFW_CONTEXT_REVISION);
    //获取OpenGL函数地址

    //渲染参数设置
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
    glAlphaFunc(GL_GREATER, 0.0); //<--这家伙在卖萌？(往后面看看，卖萌的多着呢)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glHint(GL_FOG_HINT, GL_FASTEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
    if (Multisample != 0) glEnable(GL_MULTISAMPLE_ARB);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glColor4f(0.0, 0.0, 0.0, 1.0);
    TextRenderer::BuildFont(windowwidth, windowheight);
    TextRenderer::setFontColor(1.0, 1.0, 1.0, 1.0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearDepth(1.0);
    glGenBuffersARB(1, &World::EmptyBuffer);
    if (Renderer::AdvancedRender) Renderer::initShaders();
    if (vsync) glfwSwapInterval(1);
    else glfwSwapInterval(0);
}

void setupNormalFog() {
    float fogColor[4] = {skycolorR, skycolorG, skycolorB, 1.0f};
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_START, viewdistance * 16.0f - 32.0f);
    glFogf(GL_FOG_END, viewdistance * 16.0f);
}

void loadTextures() {
    //载入纹理
    Textures::Init();

    tex_select = Textures::LoadRGBATexture("./Assets/Textures/GUI/select.bmp", "");
    tex_unselect = Textures::LoadRGBATexture("./Assets/Textures/GUI/unselect.bmp", "");
    for (auto i = 0; i < 6; i++) {
        std::stringstream ss;
        ss << "./Assets/Textures/GUI/mainmenu" << i << ".bmp";
        tex_mainmenu[i] = Textures::LoadRGBTexture(ss.str());
    }

    DefaultSkin = Textures::LoadRGBATexture("./Assets/Textures/Player/skin_xiaoqiao.bmp",
                                            "./Assets/Textures/Player/skinmask_xiaoqiao.bmp");

    for (auto gloop = 1; gloop <= 10; gloop++) {
        const auto path = "./Assets/Textures/Blocks/destroy_" + itos(gloop) + ".bmp";
        DestroyImage[gloop] = Textures::LoadRGBATexture(path, path);
    }

    BlockTextures = Textures::LoadRGBATexture("./Assets/Textures/Blocks/Terrain.bmp", "./Assets/Textures/Blocks/Terrainmask.bmp");
    loadItemsTextures();
}

void WindowSizeFunc(GLFWwindow *win, int width, int height) {
    if (width < 640) width = 640;
    if (height < 360) height = 360;
    windowwidth = width;
    windowheight = height > 0 ? height : 1;
    glfwSetWindowSize(win, width, height);
    setupScreen();
}

void MouseButtonFunc(GLFWwindow *, int button, int action, int) {
    mb = 0;
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_LEFT)mb += 1;
        if (button == GLFW_MOUSE_BUTTON_RIGHT)mb += 2;
        if (button == GLFW_MOUSE_BUTTON_MIDDLE)mb += 4;
    } else mb = 0;
}

void CharInputFunc(GLFWwindow *, unsigned int c) {
    if (c >= 128) {
        const auto pwszUnicode = new wchar_t[2];
        pwszUnicode[0] = static_cast<wchar_t>(c);
        pwszUnicode[1] = '\0';
        auto pszMultiByte = static_cast<char *>(malloc(static_cast<unsigned int>(4)));
        pszMultiByte = static_cast<char *>(realloc(pszMultiByte, WCharToMByte(pszMultiByte, pwszUnicode, 4)));
        inputstr += pszMultiByte;
        free(pszMultiByte);
        delete[] pwszUnicode;
    } else inputstr += static_cast<char>(c);
}

void MouseScrollFunc(GLFWwindow *, double, double yoffset) {
    mw += static_cast<int>(yoffset);
}
