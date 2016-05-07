#include "Window.h"

const char* WindowName = "NEWorld";
const int WindowH = 800, WindowW = 600;

bool Window::init()
{
    static bool inited = false;
    if (!inited)
    {
        glfwInit();
        inited = true;
    }

    _win = glfwCreateWindow(WindowH, WindowW, WindowName, NULL, NULL);

    if (!_win)
    {
        glfwTerminate();
        return false;
    }

    setCurrent();

    return true;
}

Window& getWindow()
{
    static Window win;
    return win;
}
