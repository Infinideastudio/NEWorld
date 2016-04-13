#ifndef SETUP_H
#define SETUP_H

struct GLFWwindow;

void splashScreen();

void createWindow();

void setupScreen();

void setupNormalFog();

void loadTextures();

void WindowSizeFunc(GLFWwindow* win, int width, int height);

void MouseButtonFunc(GLFWwindow*, int button, int action, int);

void CharInputFunc(GLFWwindow*, unsigned int c);

void MouseScrollFunc(GLFWwindow*, double, double yoffset);
#endif