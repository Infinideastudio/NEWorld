#ifndef Window_h__
#define Window_h__
#include <GLFW/glfw3.h>
class Window
{
public:
    Window() : _win(nullptr) { init(); }
    bool init();
    bool isKeyPressed(int key) { return glfwGetKey(_win, key); }
    void setCurrent() { glfwMakeContextCurrent(_win); }
    GLFWwindow* getGLFWWindow() { return _win; }

private:
    GLFWwindow* _win;
};

Window& getWindow();

#endif // Window_h__
