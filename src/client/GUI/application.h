#ifndef APPLICATION_H
#define APPLICATION_H_

#include "controls.h"
#include "window.h"
#include <functional>

class Application :public UIObject
{
public:
    std::map<std::string, std::shared_ptr<Window> > windows;
    virtual void ApplicationDoBeforeLaunch();
    virtual void ApplicationDoAfterLaunch();
    virtual void ApplicationDoEnteringBackground();
    virtual void ApplicationDoEnteringForeground();
    virtual void ApplicationDoFinalizing();
    // This is the main loop!!!!!
    void AddWindow(std::shared_ptr<Window> newWin);
    void Run(int argc, char ** argv, std::shared_ptr<Window> firstWin);
};

#endif // !APPLICATION_H
