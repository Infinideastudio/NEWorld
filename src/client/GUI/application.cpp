#include "application.h"
#include <GLFW\glfw3.h>
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
    glfwSetWindowPosCallback((*newWin), [](GLFWwindow*, int, int) {});   
    glfwSetWindowSizeCallback((*newWin), [](GLFWwindow*, int, int) {});
    glfwSetWindowFocusCallback((*newWin), [](GLFWwindow*, int) {});
    glfwSetMouseButtonCallback((*newWin), [](GLFWwindow*, int, int, int) {});
    glfwSetCursorPosCallback((*newWin), [](GLFWwindow*, double, double) {});
    glfwSetCursorEnterCallback((*newWin), [](GLFWwindow*, int) {});
    glfwSetScrollCallback((*newWin), [](GLFWwindow*, double, double) {});
    glfwSetKeyCallback((*newWin), [](GLFWwindow*, int, int, int, int) {});
    glfwSetCharCallback((*newWin), [](GLFWwindow*, unsigned int) {});
    glfwSetDropCallback((*newWin), [](GLFWwindow*, int, const char**) {});
}

void Application::Run(int argc, char ** argv, std::shared_ptr<Page> firstpage)
{
    GLFWwindow* window;
    GLFWWindowPosCASSF = [this](GLFWwindow*, int, int) {};
    GLFWWindowSizeCASSF = [this](GLFWwindow*, int, int) {};
    GLFWWindowFocusCASSF = [this](GLFWwindow*, int) {};
    GLFWMouseButtonCASSF = [this](GLFWwindow*, int, int, int) {};
    GLFWCursorPosCASSF = [this](GLFWwindow*, double, double) {};
    GLFWCursorEnterCASSF = [this](GLFWwindow*, int) {};
    GLFWScrollCASSF = [this](GLFWwindow*, double, double) {};
    GLFWKeyCASSF = [this](GLFWwindow*, int, int, int, int) {};
    GLFWCharCASSF = [this](GLFWwindow*, unsigned int) {};
    GLFWDropCASSF = [this](GLFWwindow*, int, const char**) {};
    /* Initialize the library */
    if (!glfwInit());

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(480, 320, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        ;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    //glfwSetMouseButtonCallback(window);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return;
}
