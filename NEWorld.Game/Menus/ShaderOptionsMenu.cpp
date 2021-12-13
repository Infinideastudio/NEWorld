#include "Menus.h"
#include "Renderer/Renderer.h"
#include "GUI.h"
#include "AudioSystem.h"

namespace Menus {
    class ShaderOptionsMenu : public GUI::Form {
    private:
        GUI::label title;
        GUI::button enablebtn, backbtn;
        GUI::trackbar shadowresbar, shadowdistbar;

        void onLoad() override {
            title = GUI::label(GetStrbyKey("NEWorld.shaders.caption"), -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
            enablebtn = GUI::button("", -250, -10, 60, 84, 0.5, 0.5, 0.0, 0.0);
            shadowresbar = GUI::trackbar("", 120, static_cast<int>(log2(Renderer::ShadowRes) - 10) * 40 - 1, 10, 250, 60, 84, 0.5,
                                         0.5, 0.0, 0.0);
            shadowdistbar = GUI::trackbar("", 120, (Renderer::MaxShadowDist - 2) * 4 - 1, -250, -10, 96, 120, 0.5, 0.5,
                                          0.0, 0.0);
            backbtn = GUI::button(GetStrbyKey("NEWorld.render.back"), -250, 250, -44, -20, 0.5, 0.5, 1.0, 1.0);
            registerControls(5, &title, &enablebtn, &shadowresbar, &shadowdistbar, &backbtn);
            if (!Renderer::AdvancedRender) shadowresbar.enabled = shadowdistbar.enabled = false;
        }

        void onUpdate() override {
            if (enablebtn.clicked) {
                Renderer::AdvancedRender = !Renderer::AdvancedRender;
                if (Renderer::AdvancedRender) shadowresbar.enabled = shadowdistbar.enabled = true;
                else shadowresbar.enabled = shadowdistbar.enabled = false;
            }
            Renderer::ShadowRes = static_cast<int>(pow(2, (shadowresbar.barpos + 1) / 40 + 10));
            Renderer::MaxShadowDist = (shadowdistbar.barpos + 1) / 4 + 2;
            if (backbtn.clicked) {
                GUI::PopPage();
            }
            enablebtn.text = GetStrbyKey("NEWorld.shaders.enable") + BoolYesNo(Renderer::AdvancedRender);
            std::stringstream ss;
            ss << Renderer::ShadowRes;
            shadowresbar.text = GetStrbyKey("NEWorld.shaders.shadowres") + ss.str() + "x";
            ss.str("");
            ss << Renderer::MaxShadowDist;
            shadowdistbar.text = GetStrbyKey("NEWorld.shaders.distance") + ss.str();

            AudioSystem::SpeedOfSound = AudioSystem::Air_SpeedOfSound;
            //EFX::EAXprop = Generic;
            //EFX::UpdateEAXprop();
            float Pos[] = {0.0f, 0.0f, 0.0f};
            AudioSystem::Update(Pos, false, false, Pos, false, false);
        }
    };

    void Shaderoptions() { GUI::PushPage(new ShaderOptionsMenu); }
}
