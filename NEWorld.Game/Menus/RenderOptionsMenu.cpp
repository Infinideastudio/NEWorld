#include "Menus.h"
#include "Renderer/Renderer.h"
#include <sstream>
#include "GUI.h"
#include "AudioSystem.h"

namespace Menus {
    class RenderOptionsMenu : public GUI::Form {
    private:
        GUI::label title;
        GUI::button smoothlightingbtn, fancygrassbtn, shaderbtn, vsyncbtn, backbtn;
        GUI::trackbar msaabar;

        void onLoad() override {
            title = GUI::label(GetStrbyKey("NEWorld.render.caption"), -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
            smoothlightingbtn = GUI::button("", -250, -10, 60, 84, 0.5, 0.5, 0.0, 0.0);
            fancygrassbtn = GUI::button("", 10, 250, 60, 84, 0.5, 0.5, 0.0, 0.0);
            msaabar = GUI::trackbar("", 120, Multisample == 0 ? 0 : static_cast<int>(log2(Multisample) - 1) * 30 - 1, 10, 250, 96,
                                    120, 0.5, 0.5, 0.0, 0.0);
            shaderbtn = GUI::button(GetStrbyKey("NEWorld.render.shaders"), -250, -10, 132, 156, 0.5, 0.5, 0.0, 0.0);
            vsyncbtn = GUI::button("", 10, 250, 132, 156, 0.5, 0.5, 0.0, 0.0);
            backbtn = GUI::button(GetStrbyKey("NEWorld.render.back"), -250, 250, -44, -20, 0.5, 0.5, 1.0, 1.0);
            registerControls(7, &title, &smoothlightingbtn, &fancygrassbtn, &msaabar, &shaderbtn,
                             &vsyncbtn, &backbtn);
        }

        void onUpdate() override {
            glfwGetMonitorPhysicalSize(glfwGetPrimaryMonitor(), &GUI::nScreenWidth,
                                       &GUI::nScreenHeight);
            if (smoothlightingbtn.clicked) SmoothLighting = !SmoothLighting;
            if (fancygrassbtn.clicked) NiceGrass = !NiceGrass;
            if (msaabar.barpos == 0) Multisample = 0;
            else Multisample = 1 << ((msaabar.barpos + 1) / 30 + 1);
            if (shaderbtn.clicked) Shaderoptions();
            if (vsyncbtn.clicked) {
                vsync = !vsync;
                if (vsync) glfwSwapInterval(1);
                else glfwSwapInterval(0);
            }
            if (backbtn.clicked) GUI::PopPage();
            std::stringstream ss;
            ss << Multisample;
            smoothlightingbtn.text = GetStrbyKey("NEWorld.render.smooth") + BoolEnabled(SmoothLighting);
            fancygrassbtn.text = GetStrbyKey("NEWorld.render.grasstex") + BoolYesNo(NiceGrass);
            msaabar.text = GetStrbyKey("NEWorld.render.multisample") +
                           (Multisample != 0 ? ss.str() + "x" : BoolEnabled(false));
            vsyncbtn.text = GetStrbyKey("NEWorld.render.vsync") + BoolEnabled(vsync);

            AudioSystem::SpeedOfSound = AudioSystem::Air_SpeedOfSound;
            //EFX::EAXprop = Generic;
            //EFX::UpdateEAXprop();
            float Pos[] = {0.0f, 0.0f, 0.0f};
            AudioSystem::Update(Pos, false, false, Pos, false, false);
        }
    };

    void Renderoptions() { GUI::PushPage(new RenderOptionsMenu); }
}
