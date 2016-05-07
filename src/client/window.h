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
    bool isKeyPressed(int key) const { return glfwGetKey(_win, key); }
    void setCurrentDraw() const { glfwMakeContextCurrent(_win); }
    operator GLFWwindow*() const { return _win; }
};

#endif // !WINDOW_H_
