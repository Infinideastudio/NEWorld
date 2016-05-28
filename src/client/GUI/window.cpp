/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "window.h"
#include <functional>

constexpr char* WindowName = "NEWorld";
constexpr int WindowH = 852, WindowW = 480;
std::function<void(GLFWwindow*, int button, int action, int)> mouseButtonCallback;
void glfwMouseButtonCallback(GLFWwindow* win, int button, int action, int val)
{
    mouseButtonCallback(win, button, action, val);
}

void glfwSetMouseButtonCallbackHelper(GLFWwindow* win, std::function<void(GLFWwindow*, int button, int action, int)> func)
{
    mouseButtonCallback = func;
    glfwSetMouseButtonCallbackHelper(win, glfwMouseButtonCallback);
}


bool Window::init()
{
    static bool inited = false;
    if (!inited)
    {
        glfwInit();
        inited = true;
    }

    m_win = glfwCreateWindow(WindowH, WindowW, WindowName, nullptr, nullptr);

    if (!m_win)
    {
        glfwTerminate();
        return false;
    }
    makeCurrentDrawTarget();

    glfwSetMouseButtonCallbackHelper(m_win, [this](GLFWwindow*, int button, int action, int)
    {
        MouseButton b;
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            button = MouseButton::Left;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            button = MouseButton::Right;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            button = MouseButton::Middle;
            break;
        case GLFW_MOUSE_BUTTON_5:
            button = MouseButton::Preserved1;
            break;
        case GLFW_MOUSE_BUTTON_6:
            button = MouseButton::Preserved2;
            break;
        default:
            break;
        }
        m_pages.top()->content->mouseButtonFunc(b, (ButtonAction)action); //TODO: fixit(bÎ´³õÊ¼»¯)
        return;
    });
    return true;
}

void Window::pushNavigationOperation(const NavigationOperation& operation)
{
    m_operationQueries.push(operation);
}

void Window::processNavigationOperations()
{
    while (!m_operationQueries.empty())
    {
        NavigationOperation curr = m_operationQueries.front();
        m_operationQueries.pop();
        switch (curr.operation)
        {
        case NavigationOperationTypes::PushPage:
            m_pages.push(curr.arg);
            break;
        case NavigationOperationTypes::PopPage:
            m_pages.pop();
            break;
        case NavigationOperationTypes::ClearPages:
            while (!m_pages.empty()) m_pages.pop();
            break;
        case NavigationOperationTypes::BackToFrontPage:
            assert(m_pages.size() >= 1);
            while (m_pages.size() > 1) m_pages.pop();
            break;
        default:
            break;
        }

    }
}
