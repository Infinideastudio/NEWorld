#pragma once

struct GLFWwindow;

void SplashScreen();

void createWindow();

void SetupScreen();

void setupNormalFog();

void LoadTextures();

void WindowSizeFunc(GLFWwindow* win, int width, int height);

void MouseButtonFunc(GLFWwindow*, int button, int action, int);

void CharInputFunc(GLFWwindow*, unsigned int c);
