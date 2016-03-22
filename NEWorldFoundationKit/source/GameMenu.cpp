#include "Menus.h"
#include "TextRenderer.h"
#include "World.h"
#include "Setup.h"

namespace Menus
{
class GameMenu :public GUI::Form
{
private:
    GUI::label title;
    GUI::button resumebtn, exitbtn;

    void onLoad()
    {
        glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        title = GUI::label(GetStrbyKey("NEWorld.pause.caption"), -225, 225, 0, 16, 0.5, 0.5, 0.25, 0.25);
        resumebtn = GUI::button(GetStrbyKey("NEWorld.pause.continue"), -200, 200, -35, -3, 0.5, 0.5, 0.5, 0.5);
        exitbtn = GUI::button(GetStrbyKey("NEWorld.pause.back"), -200, 200, 3, 35, 0.5, 0.5, 0.5, 0.5);
        registerControls(3, &title, &resumebtn, &exitbtn);
    }

    void onUpdate()
    {
        MutexUnlock(Mutex);
        //Make update thread realize that it should pause
        MutexLock(Mutex);
        if (resumebtn.clicked)
        {
            GUI::PopPage();
            glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glDepthFunc(GL_LEQUAL);
            glEnable(GL_CULL_FACE);
            setupNormalFog();
            double dmx, dmy;
            glfwGetCursorPos(MainWindow, &dmx, &dmy);
            mx = (int)(dmx / stretch), my = (int)(dmy / stretch);
            updateThreadPaused = false;
        }
        AudioSystem::GUIUpdate();
        if (exitbtn.clicked)
        {
            GUI::BackToMain();
            updateThreadPaused = false;
        }
    }
};

void gamemenu()
{
    GUI::PushPage(new GameMenu);
}

}
