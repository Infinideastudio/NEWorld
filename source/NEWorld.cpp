#include "Definitions.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include "World.h"
#include "GUI.h"
#include "Globalization.h"
#include "Setup.h"
#include "AudioSystem.h"
#include "Textures.h"
#include "../PluginSDK/src/pluginsdk.h"
#include "../PluginSDK/src/export_variables.h"

template<typename T>
void loadoption(std::map<string, string> &m, const char* name, T &value)
{
    if (m.find(name) == m.end()) return;
    std::stringstream ss;
    ss << m[name];
    ss >> value;
}

template<typename T>
void saveoption(std::ofstream &out, const char* name, T &value)
{
    out << string(name) << " " << value << endl;
}

void LoadOptions()
{
    std::map<string, string> options;
    std::ifstream filein("options.ini", std::ios::in);
    if (!filein.is_open()) return;
    string name, value;
    while (!filein.eof())
    {
        filein >> name >> value;
        options[name] = value;
    }
    filein.close();
    loadoption(options, "Language", Globalization::Cur_Lang);
    loadoption(options, "FOV", FOVyNormal);
    loadoption(options, "RenderDistance", ViewDistance);
    loadoption(options, "Sensitivity", mousemove);
    loadoption(options, "CloudWidth", CloudWidth);
    loadoption(options, "SmoothLighting", SmoothLighting);
    loadoption(options, "FancyGrass", NiceGrass);
    loadoption(options, "MergeFaceRendering", MergeFace);
    loadoption(options, "MultiSample", Multisample);
    loadoption(options, "AdvancedRender", Renderer::AdvancedRender);
    loadoption(options, "ShadowMapRes", Renderer::ShadowRes);
    loadoption(options, "ShadowDistance", Renderer::MaxShadowDist);
    loadoption(options, "VerticalSync", vsync);
    loadoption(options, "GUIBackgroundBlur", GUIScreenBlur);
    loadoption(options, "ppistretch", ppistretch);
    loadoption(options, "GainOfBGM", AudioSystem::BGMGain);
    loadoption(options, "GainOfSound", AudioSystem::SoundGain);
}

void SaveOptions()
{
    std::map<string, string> options;
    std::ofstream fileout("options.ini", std::ios::out);
    if (!fileout.is_open()) return;
    saveoption(fileout, "Language", Globalization::Cur_Lang);
    saveoption(fileout, "FOV", FOVyNormal);
    saveoption(fileout, "RenderDistance", ViewDistance);
    saveoption(fileout, "Sensitivity", mousemove);
    saveoption(fileout, "CloudWidth", CloudWidth);
    saveoption(fileout, "SmoothLighting", SmoothLighting);
    saveoption(fileout, "FancyGrass", NiceGrass);
    saveoption(fileout, "MergeFaceRendering", MergeFace);
    saveoption(fileout, "MultiSample", Multisample);
    saveoption(fileout, "AdvancedRender", Renderer::AdvancedRender);
    saveoption(fileout, "ShadowMapRes", Renderer::ShadowRes);
    saveoption(fileout, "ShadowDistance", Renderer::MaxShadowDist);
    saveoption(fileout, "VerticalSync", vsync);
    saveoption(fileout, "GUIBackgroundBlur", GUIScreenBlur);
    saveoption(fileout, "ppistretch", ppistretch);
    saveoption(fileout, "GainOfBGM", AudioSystem::BGMGain);
    saveoption(fileout, "GainOfSound", AudioSystem::SoundGain);
    fileout.close();
}

int main()
{
    LoadOptions();
    Globalization::Load();

#ifdef NEWORLD_TARGET_WINDOWS
    _mkdir("Worlds");
    _mkdir("Screenshots");
#elif NEWORLD_TARGET_MACOSX
    mkdir("Worlds", 644);
    mkdir("Screenshots", 644);
#endif
    RandomGeneratorInit();
    glfwInit();
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
    SetupScreen();
    glDisable(GL_CULL_FACE);
    
    //Show the splash screen
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

    //Load textures
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
#ifdef NEWORLD_TARGET_WINDOWS
    //Init plugin sdk
    init([](int x, int y, int z) -> unsigned short
    {
        return World::getblock(x, y, z).ID;
    }, [](int x, int y, int z, bool val) -> void
    {
        World::updateblock(x, y, z, val);
    });
    load_plugins(L"plugins.ini");
#endif
    AudioSystem::Init();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_LINE_SMOOTH);
    GUI::clearTransition();
    GUI::BackToMain();

    //Major part
    glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_CULL_FACE);
    GUI::AppStart();

    //Clean
    World::destroyAllChunks();
#ifdef NEWORLD_TARGET_WINDOWS
    unload_plugins();
#endif
    glfwTerminate();
    AudioSystem::UnInit();
    RandomGeneratorUninit();
    return 0;
}
