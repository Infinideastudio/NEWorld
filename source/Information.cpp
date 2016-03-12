#include "Menus.h"
#include "TextRenderer.h"
#include "World.h"
#include "Setup.h"

string Str[] =
{
    "NEWorld Main Version:" + MAJOR_VERSION + MINOR_VERSION + EXT_VERSION ,
    "CopyLeft 2016 Infinideastudio, No Rights Reserved" ,
    "Welcome to develope with us!",
    "Contributers:" ,
    "qiaozhanrong,Null,SuperSodaSea,Null,DREAMWORLDVOID," ,
    "jacky8399,0u0,jelawat地鼠,HydroH,Michael R,dtcxzyw" ,
    "" ,
    "PS: Since this is a version in development, we welcome any type of suggestion or question.",
    "Anyone is welcomed to send issues on the following project site:",
    "https://github.com/Infinideastudio/NEWorld",
    "You can submit bug reports or request new features there.",
    "If you have any problems, please contact us.",
    "Thank you very much!"
};

namespace Menus
{

class Info :public GUI::Form
{
private:
    GUI::label title;
    GUI::button backbtn;

    void onLoad()
    {
        backbtn = GUI::button(GetStrbyKey("NEWorld.language.back"), -250, 250, -44, -20, 0.5, 0.5, 1.0, 1.0);
        registerControls(1, &backbtn);
    }

    void onRender()
    {
        for (int i = 0; i < 13; ++i)
            TextRenderer::renderString(10, 10+20*i, Str[i]);
    }

    void onUpdate()
    {
        AudioSystem::GUIUpdate();
        if (backbtn.clicked)
            GUI::PopPage();
    }
};

void Information()
{
    GUI::PushPage(new Info);
}

}
