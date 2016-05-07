#ifndef WINDOW_H_
#define WINDOW_H_

#include <GLFW/glfw3.h>

class Window
{
private:
    GLFWwindow* _win;

public:
    Window() : _win(nullptr) { init(); }
    bool init();
    bool isKeyPressed(int key) { return glfwGetKey(_win, key); }
    void setCurrentDraw() { glfwMakeContextCurrent(_win); }
    operator GLFWwindow*() { return _win; }
};

#endif // !WINDOW_H_
