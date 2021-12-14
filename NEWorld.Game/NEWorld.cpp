//==============================   Initialize   ================================//
//==============================初始化(包括闪屏)================================//

#include "Definitions.h"
#include "Renderer.h"
#include "Universe/World/World.h"
#include "Globalization.h"
#include "Setup.h"
#include "GUI/GUI.h"
#include "AudioSystem.h"
#include <iostream>
#include "System/MessageBus.h"
#include "System/FileSystem.h"
#include "GUI/Menus/Menus.h"
#include "Common/Logger.h"
#include "GUI/Noesis.h"
#include "GameSettings.h"
void loadOptions();

void saveOptions();

//==============================  Main Program  ================================//
//==============================     主程序     ================================//

void ApplicationBeforeLaunch() {
    setlocale(LC_ALL, "zh_CN.UTF-8");
    GameSettings::getInstance().loadOptions();
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
