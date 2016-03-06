#include "Menus.h"
#include "TextRenderer.h"

namespace Menus {
	class GUIOptionsMenu :public GUI::Form {
	private:
		GUI::label title, ppistat;
		GUI::button fontbtn, blurbtn, ppistretchbtn, backbtn;
		void onLoad() {
			title = GUI::label(GetStrbyKey("NEWorld.gui.caption"), -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
			fontbtn = GUI::button("", -250, -10, 60, 84, 0.5, 0.5, 0.0, 0.0);
			blurbtn = GUI::button("", 10, 250, 60, 84, 0.5, 0.5, 0.0, 0.0);
			ppistretchbtn = GUI::button(GetStrbyKey("NEWorld.gui.stretch"), -250, -10, 96, 120, 0.5, 0.5, 0.0, 0.0);
			ppistat = GUI::label("", -250, 250, 120, 144, 0.5, 0.5, 0.0, 0.0);
			backbtn = GUI::button(GetStrbyKey("NEWorld.gui.back"), -250, 250, -44, -20, 0.5, 0.5, 1.0, 1.0);
			registerControls(6, &title, &fontbtn, &blurbtn, &ppistretchbtn, &ppistat, &backbtn);
			fontbtn.enabled = false;
		}
		void onUpdate() {
			//if (fontbtn.clicked) TextRenderer::useUnicodeASCIIFont = !TextRenderer::useUnicodeASCIIFont;
			if (blurbtn.clicked) GUIScreenBlur = !GUIScreenBlur;
			if (ppistretchbtn.clicked) {
				if (stretch == 1.0) GUI::InitStretch();
				else GUI::EndStretch();
			}
			AudioSystem::GUIUpdate();
			if (backbtn.clicked) GUI::PopPage();
			//fontbtn.text = GetStrbyKey("NEWorld.gui.unicode") + BoolYesNo(TextRenderer::useUnicodeASCIIFont);
			fontbtn.text = GetStrbyKey("NEWorld.gui.unicode") + BoolYesNo(true);
			blurbtn.text = GetStrbyKey("NEWorld.gui.blur") + BoolEnabled(GUIScreenBlur);
			int vmc;
			const GLFWvidmode* m = glfwGetVideoModes(glfwGetPrimaryMonitor(), &vmc);
			ppistat.text = "phy:" + Var2Str(GUI::nScreenWidth) + "x" + Var2Str(GUI::nScreenHeight) +
				" scr:" + Var2Str(m[vmc - 1].width) + "x" + Var2Str(m[vmc - 1].height) +
				" win:" + Var2Str(windowwidth) + "x" + Var2Str(windowheight);
		}
	};
	void GUIoptions() { GUI::PushPage(new GUIOptionsMenu); }
}
