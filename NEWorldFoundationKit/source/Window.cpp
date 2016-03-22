#include "Major/NEWorldFoundationKit.h"

namespace NWUIK
{
Window::Window()
{
}

Window::~Window()
{
}

void Window::PushPage(Form* View)
{
    ViewOps.push_back({ 1, View });
    HaveRequest = true;
}

void Window::PopPage()
{
    ViewOps.push_back({ 2, nullptr });
    HaveRequest = true;
}

void Window::BackToMain()
{
    ViewOps.push_back({ 3, nullptr });
    HaveRequest = true;
}

void Window::PopView()
{
    (*ViewStack.begin())->onLeave();
    delete ViewStack[0];
    ViewStack.pop_front();
}

void Window::ClearStack()
{
    ViewOps.push_back({ 4, nullptr });
    HaveRequest = true;
}

void Window::ProcessRequests()      //Process the op deque
{
    for (std::deque<PageOpRq>::iterator i = ViewOps.begin(); i != ViewOps.end(); i++)
    {
        switch (i->Op)
        {
        case 1:
            ViewStack.push_front(i->Page);
            (*ViewStack.begin())->onLoad();
            break;
        case 2:
            PopView();
            break;
        case 3:
            while (ViewStack.size() > 0) PopView();
            ViewStack.push_front(GetMain());
            (*ViewStack.begin())->onLoad();
            break;
        case 4:
            while (ViewStack.size() > 0) PopView();
            break;

        }
    }
    ViewOps.clear();
    HaveRequest = false;
}

void Window::Start()
{
    glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_CULL_FACE);
    ProcessRequests();
    TextRenderer::setFontColor(1.0, 1.0, 1.0, 1.0);
    while (ViewStack.size() > 0)
    {
        (*ViewStack.begin())->singleloop();
        if (HaveRequest) ProcessRequests();
        if (glfwWindowShouldClose(MainWindow))
        {
            while (ViewStack.size() > 0) PopView();
        }
    }
    //CleanUp();
}
}
