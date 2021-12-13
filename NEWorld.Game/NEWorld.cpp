//==============================   Initialize   ================================//
//==============================初始化(包括闪屏)================================//

#include "Definitions.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include "Universe/World/World.h"
#include "Globalization.h"
#include "Setup.h"
#include "GUI/GUI.h"
#include "AudioSystem.h"
#include <iostream>
#include <fstream>
#include <map>
#include "System/MessageBus.h"
#include "System/FileSystem.h"
#include "GUI/Menus/Menus.h"
#include "Common/Logger.h"
#include "GUI/Noesis.h"
void loadOptions();

void saveOptions();

//==============================  Main Program  ================================//
//==============================     主程序     ================================//

void ApplicationBeforeLaunch() {
    setlocale(LC_ALL, "zh_CN.UTF-8");
    loadOptions();
    Globalization::Load();
    NEWorld::filesystem::create_directories("./Configs");
    NEWorld::filesystem::create_directories("./Worlds");
    NEWorld::filesystem::create_directories("./Screenshots");
    NEWorld::filesystem::create_directories("./Mods");
    GUI::noesisSetup();
}

void ApplicationAfterLaunch() {
    loadTextures();
    //初始化音频系统
    AudioSystem::Init();
}

int main() {
    auto test = MessageBus::Default().Get<NullArg>("TEST");
    {
        auto del = test->Listen([](void*, const NullArg&) noexcept { std::cout << "Test Invoke" << std::endl; });
        test->Send(nullptr, 0);
	}
    test->Send(nullptr, 0);
    ApplicationBeforeLaunch();
    windowwidth = defaultwindowwidth;
    windowheight = defaultwindowheight;
    if (glfwInit() == 1) {
        infostream << "Initialize GLFW";
    } else {
        infostream << "GLFW initialization failed";
    }
    createWindow();
    setupScreen();
    glDisable(GL_CULL_FACE);
    //splashScreen();
    ApplicationAfterLaunch();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_LINE_SMOOTH);
    GUI::pushScene(Menus::startMenu());
    GUI::appStart();
    //结束程序，删了也没关系 ←_←（吐槽FB和glfw中）
    //不对啊这不是FB！！！这是正宗的C++！！！！！！
    //楼上的楼上在瞎说！！！别信他的！！！
    //……所以你是不是应该说“吐槽C艹”中？——地鼠
    GUI::noesisShutdown();
    glfwTerminate();
    //反初始化音频系统
    AudioSystem::UnInit();
    return 0;
}

void AppCleanUp() {
    World::saveAllChunks();
    World::destroyAllChunks();
}

template<typename T>
void loadoption(std::map<std::string, std::string> &m, const char *name, T &value) {
    if (m.find(name) == m.end()) return;
    std::stringstream ss;
    ss << m[name];
    ss >> value;
}

void loadOptions() {
    std::map<std::string, std::string> options;
    std::ifstream filein("./Configs/options.ini", std::ios::in);
    if (!filein.is_open()) return;
    std::string name, value;
    while (!filein.eof()) {
        filein >> name >> value;
        options[name] = value;
    }
    filein.close();
    loadoption(options, "Language", Globalization::Cur_Lang);
    loadoption(options, "FOV", FOVyNormal);
    loadoption(options, "RenderDistance", viewdistance);
    loadoption(options, "Sensitivity", mousemove);
    loadoption(options, "CloudWidth", cloudwidth);
    loadoption(options, "SmoothLighting", SmoothLighting);
    loadoption(options, "FancyGrass", NiceGrass);
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

template<typename T>
void saveoption(std::ofstream &out, const char *name, T &value) {
    out << std::string(name) << " " << value << std::endl;
}

void saveOptions() {
    std::map<std::string, std::string> options;
    std::ofstream fileout("./Configs/options.ini", std::ios::out);
    if (!fileout.is_open()) return;
    saveoption(fileout, "Language", Globalization::Cur_Lang);
    saveoption(fileout, "FOV", FOVyNormal);
    saveoption(fileout, "RenderDistance", viewdistance);
    saveoption(fileout, "Sensitivity", mousemove);
    saveoption(fileout, "CloudWidth", cloudwidth);
    saveoption(fileout, "SmoothLighting", SmoothLighting);
    saveoption(fileout, "FancyGrass", NiceGrass);
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
