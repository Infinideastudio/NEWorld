#include "Menus.h"
#include "World.h"
#include "GameView.h"

namespace Menus
{
class CreateWorldMenu :public GUI::Form
{
private:
    bool worldnametbChanged;
    GUI::label title;
    GUI::textbox worldnametb;
    GUI::button okbtn, backbtn;
    void onLoad()
    {
        title = GUI::label(GetStrbyKey("NEWorld.create.caption"), -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
        worldnametb = GUI::textbox(GetStrbyKey("NEWorld.create.inputname"), -250, 250, 48, 72, 0.5, 0.5, 0.0, 0.0);
        okbtn = GUI::button(GetStrbyKey("NEWorld.create.ok"), -250, 250, 84, 120, 0.5, 0.5, 0.0, 0.0);
        backbtn = GUI::button(GetStrbyKey("NEWorld.create.back"), -250, 250, -44, -20, 0.5, 0.5, 1.0, 1.0);
        registerControls(4, &title, &worldnametb, &okbtn, &backbtn);
        inputstr = "";
        okbtn.enabled = false;
        worldnametbChanged = false;
    }
    void onUpdate()
    {
        if (worldnametb.pressed && !worldnametbChanged)
        {
            worldnametb.text = "";
            worldnametbChanged = true;
        }
        if (worldnametb.text == "" || !worldnametbChanged) okbtn.enabled = false;
        else okbtn.enabled = true;
        if (okbtn.clicked)
        {
            if (worldnametb.text != "")
            {
                World::worldname = worldnametb.text;
                GUI::ClearStack();
                GameView();
            }
            else
            {
                GUI::PopPage();
            }
        }
        AudioSystem::GUIUpdate();
        if (backbtn.clicked) GUI::PopPage();
        inputstr = "";
    }
};
void createworldmenu()
{
    GUI::PushPage(new CreateWorldMenu);
}
}