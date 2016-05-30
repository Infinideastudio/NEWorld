#include "application.h"
#include <GLFW/glfw3.h>
#include <functional>

using GLFWWindowPosASSF = std::function<void(GLFWwindow*, int, int)>;
using GLFWWindowSizeASSF = std::function<void(GLFWwindow*, int, int)>;
using GLFWWindowFocusASSF = std::function<void(GLFWwindow*, int)>;
using GLFWMouseButtonASSF = std::function<void(GLFWwindow*, int, int, int)>;
using GLFWCursorPosASSF = std::function<void(GLFWwindow*, double, double)>;
using GLFWCursorEnterASSF = std::function<void(GLFWwindow*, int)>;
using GLFWScrollASSF = std::function<void(GLFWwindow*, double, double)>;
using GLFWKeyASSF = std::function<void(GLFWwindow*, int, int, int, int)>;
using GLFWCharASSF = std::function<void(GLFWwindow*, unsigned int)>;
using GLFWDropASSF = std::function<void(GLFWwindow*, int, const char**)>;

GLFWWindowPosASSF GLFWWindowPosCASSF;
GLFWWindowSizeASSF GLFWWindowSizeCASSF;
GLFWWindowFocusASSF GLFWWindowFocusCASSF;
GLFWMouseButtonASSF GLFWMouseButtonCASSF;
GLFWCursorPosASSF GLFWCursorPosCASSF;
GLFWCursorEnterASSF GLFWCursorEnterCASSF;
GLFWScrollASSF GLFWScrollCASSF;
GLFWKeyASSF GLFWKeyCASSF;
GLFWCharASSF GLFWCharCASSF;
GLFWDropASSF GLFWDropCASSF;

void Application::AddWindow(std::shared_ptr<Window> newWin)
{
    newWin->init();
    glfwSetWindowPosCallback((*newWin), [](GLFWwindow* win, int xpos, int ypos) 
    {
        GLFWWindowPosCASSF(win, xpos, ypos);
    });   
    glfwSetWindowSizeCallback((*newWin), [](GLFWwindow* win, int width, int height)
    {
        GLFWWindowSizeCASSF(win, width, height);
    });
    glfwSetWindowFocusCallback((*newWin), [](GLFWwindow* win, int focused)
    {
        GLFWWindowFocusCASSF(win, focused);
    });
    glfwSetMouseButtonCallback((*newWin), [](GLFWwindow* win, int button, int action, int mods) 
    {
        GLFWMouseButtonCASSF(win, button, action, mods);
    });
    glfwSetCursorPosCallback((*newWin), [](GLFWwindow* win, double xpos, double ypos) 
    {
        GLFWCursorPosCASSF(win, xpos, ypos);
    });
    glfwSetCursorEnterCallback((*newWin), [](GLFWwindow* win, int entered) 
    {
        GLFWCursorEnterCASSF(win, entered);
    });
    glfwSetScrollCallback((*newWin), [](GLFWwindow* win, double xoffset, double yoffset) 
    {
        GLFWScrollCASSF(win, xoffset, yoffset);
    });
    glfwSetKeyCallback((*newWin), [](GLFWwindow* win, int key, int scancode, int action, int mods) 
    {
        GLFWKeyCASSF(win, key, scancode, action, mods);
    });
    glfwSetCharCallback((*newWin), [](GLFWwindow* win, unsigned int codepoint) 
    {
        GLFWCharCASSF(win, codepoint);
    });
    glfwSetDropCallback((*newWin), [](GLFWwindow* win, int count, const char** paths) 
    {
        GLFWDropCASSF(win, count, paths);
    });
    windows.insert({ "",newWin });
}

void Application::Run(int argc, char ** argv, std::shared_ptr<Window> firstWin)
{
    sigTerminiate = false;
    ApplicationDoBeforeLaunch();
    GLFWWindowPosCASSF = [this](GLFWwindow* win, int xpos, int ypos) 
    {
    };
    GLFWWindowSizeCASSF = [this](GLFWwindow* win, int width, int height) 
    {
    };
    GLFWWindowFocusCASSF = [this](GLFWwindow* win, int focused) 
    {
    };
    GLFWMouseButtonCASSF = [this](GLFWwindow* win, int button, int action, int mods) 
    {
    };
    GLFWCursorPosCASSF = [this](GLFWwindow* win, double xpos, double ypos) 
    {
    };
    GLFWCursorEnterCASSF = [this](GLFWwindow* win, int entered) 
    {
    };
    GLFWScrollCASSF = [this](GLFWwindow* win, double xoffset, double yoffset) 
    {
    };
    GLFWKeyCASSF = [this](GLFWwindow*, GLFWwindow* win, int key, int scancode, int action, int mods) 
    {
    };
    GLFWCharCASSF = [this](GLFWwindow* win, unsigned int codepoint) 
    {
    };
    GLFWDropCASSF = [this](GLFWwindow* win, int count, const char** paths) 
    {
    };
    // Initialize the library 
    if (!glfwInit());

    AddWindow(firstWin);
    ApplicationDoAfterLaunch();

    while (1)
    {
        for (auto win : windows)
        {
            glfwMakeContextCurrent(*(win.second));
            win.second->getCurPage()->content->render();
            glfwSwapBuffers(*(win.second));
        }
        // Poll for and process events 
        glfwPollEvents();
    }
    ApplicationDoFinalizing();
    glfwTerminate();
    return;
}
