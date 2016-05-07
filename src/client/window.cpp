#include "window.h"

const char* WindowName = "NEWorld";
const int WindowH = 852, WindowW = 480;

bool Window::init()
{
    static bool inited = false;
    if (!inited)
    {
        glfwInit();
        inited = true;
    }

    _win = glfwCreateWindow(WindowH, WindowW, WindowName, nullptr, nullptr);

    if (!_win)
    {
        glfwTerminate();
        return false;
    }

    setCurrentDraw();

    return true;
}
