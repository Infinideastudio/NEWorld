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

constexpr char* WindowName = "NEWorld";
constexpr int WindowH = 852, WindowW = 480;

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
        if (curr.operation == NavigationOperationTypes::PushPage) m_pages.push(curr.arg);
        else if (curr.operation == NavigationOperationTypes::PopPage) m_pages.pop();
        else if (curr.operation == NavigationOperationTypes::ClearPages) while (!m_pages.empty()) m_pages.pop();
        else if (curr.operation == NavigationOperationTypes::BackToFrontPage)
        {
            assert(m_pages.size() >= 1);
            while (m_pages.size() > 1) m_pages.pop();
        }
    }
}
