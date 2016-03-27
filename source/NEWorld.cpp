#include "Definitions.h"
#include "Blocks.h"
#include "Textures.h"
#include "GLProc.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include "Player.h"
#include "WorldGen.h"
#include "World.h"
#include "WorldRenderer.h"
#include "ShadowMaps.h"
#include "Particles.h"
#include "Hitbox.h"
#include "GUI.h"
#include "Menus.h"
#include "Frustum.h"
#include "Network.h"
#include "Effect.h"
#include "Items.h"
#include "Globalization.h"
#include "Command.h"
#include "ModLoader.h"
#include "Setup.h"
#include"AudioSystem.h"
void loadOptions();
void saveOptions();

void ApplicationBeforeLaunch()
{
#ifdef NEWORLD_X64
    SetDllDirectoryA("");
    SetDllDirectoryA("Dllx64");
#endif
#ifndef NEWORLD_USE_WINAPI
    setlocale(LC_ALL, "zh_CN.UTF-8");
#else
    //提交OpenGL信息
    std::ifstream postexe("Post.exe");
    if (postexe.is_open())
    {
        postexe.close();
        WinExec("Post.exe", SW_SHOWDEFAULT);
        Sleep(3000);
    }
    else postexe.close();
#endif
    loadOptions();
    Globalization::Load();

    _mkdir("Configs");
    _mkdir("Worlds");
    _mkdir("Screenshots");
    _mkdir("Mods");
}

void ApplicationAfterLaunch()
{
    loadTextures();
    Mod::ModLoader::loadMods();
    //初始化音频系统
    AudioSystem::Init();
}

int main()
{
    ApplicationBeforeLaunch();
    windowwidth = defaultwindowwidth;
    windowheight = defaultwindowheight;
    cout << "[Console][Event]Initialize GLFW" << (glfwInit() == 1 ? "" : " - Failed!") << endl;
    createWindow();
    setupScreen();
    glDisable(GL_CULL_FACE);
    splashScreen();
    ApplicationAfterLaunch();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_LINE_SMOOTH);
    GUI::clearTransition();
    //App Entrance
    GUI::BackToMain();
    GUI::AppStart();
    glfwTerminate();
    //反初始化音频系统
    AudioSystem::UnInit();
    return 0;
}

void AppCleanUp()
{
    World::saveAllChunks();
    World::destroyAllChunks();
    Mod::ModLoader::unloadMods();
}

template<typename T>
void loadoption(std::map<string, string> &m, const char* name, T &value)
{
    if (m.find(name) == m.end()) return;
    std::stringstream ss;
    ss << m[name];
    ss >> value;
}

void loadOptions()
{
    std::map<string, string> options;
    std::ifstream filein("Configs/options.ini", std::ios::in);
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
    loadoption(options, "ForceUnicodeFont", TextRenderer::UseUnicodeASCIIFont);
    loadoption(options, "GainOfBGM", AudioSystem::BGMGain);
    loadoption(options, "GainOfSound", AudioSystem::SoundGain);
}

template<typename T>
void saveoption(std::ofstream &out, const char* name, T &value)
{
    out << string(name) << " " << value << endl;
}

void saveOptions()
{
    std::map<string, string> options;
    std::ofstream fileout("Configs/options.ini", std::ios::out);
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
    saveoption(fileout, "ForceUnicodeFont", TextRenderer::UseUnicodeASCIIFont);
    saveoption(fileout, "GainOfBGM", AudioSystem::BGMGain);
    saveoption(fileout, "GainOfSound", AudioSystem::SoundGain);
    fileout.close();
}
