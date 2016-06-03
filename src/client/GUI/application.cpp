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
    winByGLFWWin.insert({(*newWin), newWin});
}

void Application::Run(int argc, char ** argv, std::shared_ptr<Window> firstWin)
{
    sigTerminate = false;
    ApplicationDoBeforeLaunch();
    GLFWWindowPosCASSF = [this](GLFWwindow* win, int xpos, int ypos) 
    {
    };
    GLFWWindowSizeCASSF = [this](GLFWwindow* win, int width, int height) 
    {
    };
    GLFWWindowFocusCASSF = [this](GLFWwindow* win, int focused) 
    {
        FocusOp op;
        switch (focused)
        {
        case GL_TRUE:
            op = Gain;
            break;
        case GL_FALSE:
            op = Lose;
            break;
        }
        winByGLFWWin[win]->getCurPage()->content->focusFunc(op);
    };
    GLFWMouseButtonCASSF = [this](GLFWwindow* win, int button, int action, int mods) 
    {
        MouseButton b;
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            b = MouseButton::Left;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            b = MouseButton::Right;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            b = MouseButton::Middle;
            break;
        case GLFW_MOUSE_BUTTON_5:
            b = MouseButton::Preserved1;
            break;
        case GLFW_MOUSE_BUTTON_6:
            b = MouseButton::Preserved2;
            break;
        default:
            break;
        }
        winByGLFWWin[win]->getCurPage()->content->mouseButtonFunc(b, (ButtonAction)action); 
    };
    GLFWCursorPosCASSF = [this](GLFWwindow* win, double xpos, double ypos) 
    {
        winByGLFWWin[win]->getCurPage()->content->cursorPosFunc(xpos, ypos);
    };
    GLFWCursorEnterCASSF = [this](GLFWwindow* win, int entered) 
    {
        CursorOp op;
        switch (entered)
        {
        case GL_TRUE:
            op = Enter;
            break;
        case GL_FALSE:
            op = Leave;
            break;
        }
        winByGLFWWin[win]->getCurPage()->content->crusorEnterFunc(op);
    };
    GLFWScrollCASSF = [this](GLFWwindow* win, double xoffset, double yoffset) 
    {
        winByGLFWWin[win]->getCurPage()->content->scrollFunc(xoffset, yoffset); 
    };
    GLFWKeyCASSF = [this](GLFWwindow* win, int key, int scancode, int action, int mods)
    {
        winByGLFWWin[win]->getCurPage()->content->keyFunc(key, (ButtonAction)action);
    };
    GLFWCharCASSF = [this](GLFWwindow* win, unsigned int codepoint) 
    {
        winByGLFWWin[win]->getCurPage()->content->charInputFunc((wchar_t) codepoint);
    };
    GLFWDropCASSF = [this](GLFWwindow* win, int count, const char** paths) 
    {
        winByGLFWWin[win]->getCurPage()->content->dropFunc(count, paths);
    };
    // Initialize the library 
    if (!glfwInit());

    AddWindow(firstWin);
    ApplicationDoAfterLaunch();

    while (sigTerminate)
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
