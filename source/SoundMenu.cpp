#include "Menus.h"
#include "TextRenderer.h"
#include "Renderer.h"
#include"AudioSystem.h"

namespace Menus
{
class SoundMenu :public GUI::Form
{
private:
    GUI::label title;
    GUI::button backbtn;
    GUI::trackbar Musicbar,SoundBar;
    void onLoad()
    {
        title = GUI::label(GetStrbyKey("NEWorld.Sound.caption"), -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
        Musicbar = GUI::trackbar(GetStrbyKey("NEWorld.Sound.MusicGain"), 100, AudioSystem::BGMGain*300, -200, 201, 60, 84, 0.5, 0.5, 0.0, 0.0);
        SoundBar= GUI::trackbar(GetStrbyKey("NEWorld.Sound.SoundGain"), 100, AudioSystem::SoundGain*300, -200, 201, 90, 114, 0.5, 0.5, 0.0, 0.0);
        backbtn = GUI::button(GetStrbyKey("NEWorld.render.back"), -250, 250, -44, -20, 0.5, 0.5, 1.0, 1.0);
        registerControls(4, &title,&Musicbar,&SoundBar , &backbtn);
    }
    void onUpdate()
    {
        char text[100];
        AudioSystem::BGMGain = float(Musicbar.barpos) / 300.0f;
        AudioSystem::SoundGain = float(SoundBar.barpos) / 300.0f;
        sprintf_s(text, ":%d%%", Musicbar.barpos/3);
        Musicbar.text = GetStrbyKey("NEWorld.Sound.MusicGain") + text;
        sprintf_s(text, ":%d%%", SoundBar.barpos/3);
        SoundBar.text = GetStrbyKey("NEWorld.Sound.SoundGain") + text;
        AudioSystem::GUIUpdate();
        if (backbtn.clicked) GUI::PopPage();
    }
};
void Soundmenu()
{
    GUI::PushPage(new SoundMenu);
}
}
