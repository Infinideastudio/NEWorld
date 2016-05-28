#include "application.h"
#include <GLFW\glfw3.h>

void Application::Run(int argc, char ** argv, std::shared_ptr<Page> firstpage)
{
    GLFWwindow* window;

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
