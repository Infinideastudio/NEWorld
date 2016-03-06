#include "Menus.h"
#include <deque>
struct Langinfo
{
    string Symbol, EngSymbol, Name;
    GUI::button * Button;
};

namespace Menus
{
class Language :public GUI::Form
{
private:
    std::deque<Langinfo> Langs;
    GUI::label title;
    GUI::button backbtn;

    void onLoad()
    {
        Langs.clear();
        title = GUI::label(GetStrbyKey("NEWorld.language.caption"), -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
        backbtn = GUI::button(GetStrbyKey("NEWorld.language.back"), -250, 250, -44, -20, 0.5, 0.5, 1.0, 1.0);
        registerControls(2, &title, &backbtn);
        std::ifstream index("Lang/Langs.txt");
        int count;
        index >> count;
        Langinfo Info;
        for (int i = 0; i < count; i++)
        {
            index >> Info.Symbol;
            std::ifstream LF("Lang/" + Info.Symbol + ".lang");
            getline(LF, Info.EngSymbol);
            getline(LF, Info.Name);
            LF.close();
            Info.Button = new GUI::button(Info.EngSymbol + "--" + Info.Name, -200, 200, i * 36 + 42, i * 36 + 72, 0.5, 0.5, 0.0, 0.0);
            registerControls(1, Info.Button);
            Langs.push_back(Info);
        }
        index.close();
    }

    void onUpdate()
    {
        AudioSystem::GUIUpdate();
        if (backbtn.clicked) GUI::PopPage();
        for (size_t i = 0; i < Langs.size(); i++)
        {
            if (Langs[i].Button->clicked)
            {
                GUI::PopPage();
                if (Globalization::Cur_Lang != Langs[i].Symbol)
                {
                    Globalization::LoadLang(Langs[i].Symbol);
                    GUI::BackToMain();
                }
                break;
            }
        }
    }

    void onLeave()
    {
        for (size_t i = 0; i < Langs.size(); i++)
        {
            for (vector<GUI::controls*>::iterator iter = children.begin(); iter != children.end(); )
            {
                if ((*iter)->id == Langs[i].Button->id) iter = children.erase(iter);
                else ++iter;
            }
            Langs[i].Button->destroy();
            delete Langs[i].Button;
        }
    }
};
void languagemenu()
{
    GUI::PushPage(new Language);
}
}
