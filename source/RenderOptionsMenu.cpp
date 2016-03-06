#include "Menus.h"
#include "TextRenderer.h"
#include "Renderer.h"
#include <sstream>

namespace Menus {
	class RenderOptionsMenu :public GUI::Form {
	private:
		GUI::label title;
		GUI::button smoothlightingbtn, fancygrassbtn, mergefacebtn, shaderbtn, vsyncbtn, backbtn;
		GUI::trackbar msaabar;
		void onLoad() {
			title = GUI::label(GetStrbyKey("NEWorld.render.caption"), -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
			smoothlightingbtn = GUI::button("", -250, -10, 60, 84, 0.5, 0.5, 0.0, 0.0);
			fancygrassbtn = GUI::button("", 10, 250, 60, 84, 0.5, 0.5, 0.0, 0.0);
			mergefacebtn = GUI::button("", -250, -10, 96, 120, 0.5, 0.5, 0.0, 0.0);
			msaabar = GUI::trackbar("", 120, Multisample == 0 ? 0 : (int)(log2(Multisample) - 1) * 30 - 1, 10, 250, 96, 120, 0.5, 0.5, 0.0, 0.0);
			shaderbtn = GUI::button(GetStrbyKey("NEWorld.render.shaders"), -250, -10, 132, 156, 0.5, 0.5, 0.0, 0.0);
			vsyncbtn = GUI::button("", 10, 250, 132, 156, 0.5, 0.5, 0.0, 0.0);
			backbtn = GUI::button(GetStrbyKey("NEWorld.render.back"), -250, 250, -44, -20, 0.5, 0.5, 1.0, 1.0);
			registerControls(8, &title, &smoothlightingbtn, &fancygrassbtn, &mergefacebtn, &msaabar, &shaderbtn, &vsyncbtn, &backbtn);
		}
		void onUpdate() {
			glfwGetMonitorPhysicalSize(glfwGetPrimaryMonitor(), &GUI::nScreenWidth,
				&GUI::nScreenHeight);
			if (smoothlightingbtn.clicked) SmoothLighting = !SmoothLighting;
			if (fancygrassbtn.clicked) NiceGrass = !NiceGrass;
			if (mergefacebtn.clicked) MergeFace = !MergeFace;
			if (msaabar.barpos == 0) Multisample = 0;
			else Multisample = 1 << ((msaabar.barpos + 1) / 30 + 1);
			if (shaderbtn.clicked) Shaderoptions();
			if (vsyncbtn.clicked) {
				vsync = !vsync;
				if (vsync) glfwSwapInterval(1);
				else glfwSwapInterval(0);
			}
			if (backbtn.clicked) GUI::PopPage();
			std::stringstream ss; ss << Multisample;
			smoothlightingbtn.text = GetStrbyKey("NEWorld.render.smooth") + BoolEnabled(SmoothLighting);
			fancygrassbtn.text = GetStrbyKey("NEWorld.render.grasstex") + BoolYesNo(NiceGrass);
			mergefacebtn.text = GetStrbyKey("NEWorld.render.merge") + BoolEnabled(MergeFace);
			msaabar.text = GetStrbyKey("NEWorld.render.multisample") + (Multisample != 0 ? ss.str() + "x" : BoolEnabled(false));
			vsyncbtn.text = GetStrbyKey("NEWorld.render.vsync") + BoolEnabled(vsync);

			AudioSystem::GUIUpdate();;
		}
	};
	void Renderoptions() { GUI::PushPage(new RenderOptionsMenu); }
}
