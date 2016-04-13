#include "Menus.h"
#include "TextRenderer.h"
#include "World.h"
#include "Setup.h"
#include "GameView.h"
#include "Renderer.h"
#include "AudioSystem.h"
#include "Textures.h"
#include <deque>
#include <sstream>

void SaveOptions();

string Str[] =
{
    "NEWorld Main Version:" + major_version + minor_version + ext_version ,
    "Copyright (c) 2016 Infinideastudio under the Public Domain" ,
    "Welcome to develop with us!",
    "Contributers:" ,
    "qiaozhanrong,Null,SuperSodaSea,ascchrvalstr,DREAMWORLDVOID," ,
    "jacky8399,0u0,jelawat地鼠,HydroH,Michael R,dtcxzyw" ,
    "" ,
    "PS: Since this is a version in development, we welcome any type of suggestion or question.",
    "Everyone is welcomed to send issues on the following project site:",
    "https://github.com/Infinideastudio/NEWorld",
    "You can submit bug reports or request new features there.",
    "If you have any problems, please contact us.",
    "Thank you very much!"
};

namespace Menus
{
	int getDotCount(string s)
	{
		int ret = 0;
		for (size_t i = 0; i < s.size(); i++)
			if (s[i] == '.') ret++;
		return ret;
	}
	
	void MultiplayerMenu::onLoad()
	{
		title = GUI::label("                 多 人 游 戏                 ", -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
		serveriptb = GUI::textbox("输入服务器IP", -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
		runbtn = GUI::button("开启服务器", -255, 255, 20, 36, 0.5, 0.5, 0.0, 0.0);
		okbtn = GUI::button("确定", -255, 255, 20, 36, 0.5, 0.5, 0.0, 0.0);
		backbtn = GUI::button("<< 返回", -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
		inputstr = "";
		okbtn.enabled = false;
		registerControls(4, &title, &serveriptb, &runbtn, &okbtn, &backbtn);
	}
	
	void MultiplayerMenu::onUpdate()
	{
		static bool serveripChanged = false;
#ifdef NEWORLD_USE_WINAPI
		if (runbtn.clicked) WinExec("NEWorldServer.exe", SW_SHOWDEFAULT);
#endif
		if (okbtn.clicked)
		{
			serverip = serveriptb.text;
			gamebegin = true;
			multiplayer = true;
		}

		AudioSystem::GUIUpdate();
		if (backbtn.clicked) GUI::PopPage();
		if (serveriptb.pressed && !serveripChanged)
		{
			serveriptb.text = "";
			serveripChanged = true;
		}
		if (serveriptb.text == "" || !serveripChanged || getDotCount(serveriptb.text) != 3) okbtn.enabled = false;
		else okbtn.enabled = true;
		inputstr = "";
	}
	
	void GameMenu::onLoad()
	{
		glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		title = GUI::label(GetStrbyKey("NEWorld.pause.caption"), -225, 225, 0, 16, 0.5, 0.5, 0.25, 0.25);
		resumebtn = GUI::button(GetStrbyKey("NEWorld.pause.continue"), -200, 200, -35, -3, 0.5, 0.5, 0.5, 0.5);
		exitbtn = GUI::button(GetStrbyKey("NEWorld.pause.back"), -200, 200, 3, 35, 0.5, 0.5, 0.5, 0.5);
		registerControls(3, &title, &resumebtn, &exitbtn);
	}

	void GameMenu::onUpdate()
	{
		MutexUnlock(Mutex);
		//Make update thread realize that it should pause
		MutexLock(Mutex);
		if (resumebtn.clicked)
		{
			GUI::PopPage();
			glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_CULL_FACE);
			SetupNormalFog();
			double dmx, dmy;
			glfwGetCursorPos(MainWindow, &dmx, &dmy);
			mx = (int)(dmx / stretch), my = (int)(dmy / stretch);
			updateThreadPaused = false;
		}
		AudioSystem::GUIUpdate();
		if (exitbtn.clicked)
		{
			GUI::BackToMain();
			updateThreadPaused = false;
		}
	}
	
	void CreateWorldMenu::onLoad()
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
	
	void CreateWorldMenu::onUpdate()
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
	
	void MainMenu::onLoad()
	{
		title = GUI::imagebox(0.0f, 1.0f, 0.5f, 1.0f, tex_title, -256, 256, 20, 276, 0.5, 0.5, 0.0, 0.0);
		startbtn = GUI::button(GetStrbyKey("NEWorld.main.start"), -200, 200, 280, 312, 0.5, 0.5, 0.0, 0.0);
		optionsbtn = GUI::button(GetStrbyKey("NEWorld.main.options"), -200, -3, 318, 352, 0.5, 0.5, 0.0, 0.0);
		quitbtn = GUI::button(GetStrbyKey("NEWorld.main.exit"), 3, 200, 318, 352, 0.5, 0.5, 0.0, 0.0);
		info = GUI::button("i", 210, 250, 318, 352, 0.5, 0.5, 0.0, 0.0);
		registerControls(5, &title, &startbtn, &optionsbtn, &info, &quitbtn);
	}
	
	void MainMenu::onUpdate()
	{
		AudioSystem::GUIUpdate();
		if (startbtn.clicked) GUI::PushPage(new WorldMenu);
		if (gamebegin) GUI::PopPage();
		if (optionsbtn.clicked)
		{
			GUI::PushPage(new OptionsMenu);
			startbtn.text = GetStrbyKey("NEWorld.main.start");
			optionsbtn.text = GetStrbyKey("NEWorld.main.options");
			quitbtn.text = GetStrbyKey("NEWorld.main.exit");
		}
		if (info.clicked) GUI::PushPage(new Info);
		if (quitbtn.clicked) exit(0);
	}

	void Language::onLoad()
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

	void Language::onUpdate()
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

	void Language::onLeave()
	{
		for (size_t i = 0; i < Langs.size(); i++)
		{
			for (vector<GUI::control*>::iterator iter = children.begin(); iter != children.end(); )
			{
				if ((*iter)->id == Langs[i].Button->id) iter = children.erase(iter);
				else ++iter;
			}
			delete Langs[i].Button;
		}
	}
	
	void RenderOptionsMenu::onLoad()
	{
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
	
	void RenderOptionsMenu::onUpdate()
	{
		glfwGetMonitorPhysicalSize(glfwGetPrimaryMonitor(), &GUI::nScreenWidth,
								   &GUI::nScreenHeight);
		if (smoothlightingbtn.clicked) SmoothLighting = !SmoothLighting;
		if (fancygrassbtn.clicked) NiceGrass = !NiceGrass;
		if (mergefacebtn.clicked) MergeFace = !MergeFace;
		if (msaabar.barpos == 0) Multisample = 0;
		else Multisample = 1 << ((msaabar.barpos + 1) / 30 + 1);
		if (shaderbtn.clicked) GUI::PushPage(new ShaderOptionsMenu);
		if (vsyncbtn.clicked)
		{
			vsync = !vsync;
			if (vsync) glfwSwapInterval(1);
			else glfwSwapInterval(0);
		}
		if (backbtn.clicked) GUI::PopPage();
		std::stringstream ss;
		ss << Multisample;
		smoothlightingbtn.text = GetStrbyKey("NEWorld.render.smooth") + BoolEnabled(SmoothLighting);
		fancygrassbtn.text = GetStrbyKey("NEWorld.render.grasstex") + BoolYesNo(NiceGrass);
		mergefacebtn.text = GetStrbyKey("NEWorld.render.merge") + BoolEnabled(MergeFace);
		msaabar.text = GetStrbyKey("NEWorld.render.multisample") + (Multisample != 0 ? ss.str() + "x" : BoolEnabled(false));
		vsyncbtn.text = GetStrbyKey("NEWorld.render.vsync") + BoolEnabled(vsync);

		AudioSystem::GUIUpdate();;
	}
	
	void WorldMenu::onLoad()
	{
		title = GUI::label(GetStrbyKey("NEWorld.worlds.caption"), -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
		vscroll = GUI::vscroll(100, 0, 275, 295, 36, -20, 0.5, 0.5, 0.0, 1.0);
		enterbtn = GUI::button(GetStrbyKey("NEWorld.worlds.enter"), -250, -10, -80, -56, 0.5, 0.5, 1.0, 1.0);
		deletebtn = GUI::button(GetStrbyKey("NEWorld.worlds.delete"), 10, 250, -80, -56, 0.5, 0.5, 1.0, 1.0);
		backbtn = GUI::button(GetStrbyKey("NEWorld.worlds.back"), -250, 250, -44, -20, 0.5, 0.5, 1.0, 1.0);
		registerControls(5, &title, &vscroll, &enterbtn, &deletebtn, &backbtn);
		World::worldname = "";
		enterbtn.enabled = false;
		deletebtn.enabled = false;
		vscroll.defaultv = true;
	}
		
	void WorldMenu::onUpdate()
	{
		AudioSystem::GUIUpdate();

		worldcount = (int)worldnames.size();
		leftp = static_cast<int>(windowwidth / 2.0 / stretch - 250);
		midp = static_cast<int>(windowwidth / 2.0 / stretch);
		rightp = static_cast<int>(windowwidth / 2.0 / stretch + 250);
		downp = static_cast<int>(windowheight / stretch - 20);

		vscroll.barheight = (downp - 72 - 48)*(downp - 36 - 40) / (64 * worldcount + 64);
		if (vscroll.barheight > downp - 36 - 40)
		{
			vscroll.enabled = false;
			vscroll.barheight = downp - 36 - 40;
		}
		else vscroll.enabled = true;

		trs = vscroll.barpos*(64 * worldcount + 64) / (downp - 36 - 40);
		mouseon = -1;
		if (mx >= midp - 250 && mx <= midp + 250 && my >= 48 && my <= downp - 72)
		{
			for (int i = 0; i < worldcount; i++)
			{
				if (my >= 48 + i * 64 - trs&&my <= 48 + i * 64 + 60 - trs)
				{
					if (mb == 1 && mbl == 0)
					{
						chosenWorldName = worldnames[i];
						selected = i;
					}
					mouseon = i;
				}
			}
			if (my >= 48 + worldcount * 64 - trs&&my <= 48 + worldcount * 64 + 60 - trs)
			{
				if (mb == 0 && mbl == 1)
				{
					GUI::PushPage(new CreateWorldMenu);
					refresh = true;
				}
				mouseon = worldcount;
			}
		}
		if (enterbtn.clicked)
		{
			World::worldname = chosenWorldName;
			GUI::ClearStack();
			GameView();
		}
		if (deletebtn.clicked)
		{
			//删除世界文件
			system((string("rd /s/q \"Worlds\\") + chosenWorldName + "\"").c_str());
			deletebtn.clicked = false;
			World::worldname = "";
			enterbtn.enabled = false;
			deletebtn.enabled = false;
			refresh = true;
		}
		if (refresh)
		{
			worldnames.clear();
			thumbnails.clear();
			texSizeX.clear();
			texSizeY.clear();
			worldcount = 0;
			selected = -1;
			mouseon = -1;
			vscroll.barpos = 0;
			chosenWorldName = "";
			//查找所有世界存档
			long hFile = 0;
			_finddata_t fileinfo;
			if ((hFile = _findfirst("Worlds\\*", &fileinfo)) != -1)
			{
				do
				{
					if ((fileinfo.attrib &  _A_SUBDIR))
					{
						if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
						{
							worldnames.push_back(fileinfo.name);
							std::fstream file;
							file.open(("Worlds\\" + string(fileinfo.name) + "\\Thumbnail.bmp").c_str(), std::ios::in);
							thumbnails.push_back(0);
							texSizeX.push_back(0);
							texSizeY.push_back(0);
							if (file.is_open())
							{
								glGenTextures(1, &thumbnails[thumbnails.size() - 1]);
								glBindTexture(GL_TEXTURE_2D, thumbnails[thumbnails.size() - 1]);
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
								Textures::TEXTURE_RGB tmb("Worlds\\" + string(fileinfo.name) + "\\Thumbnail.bmp");
								glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tmb.sizeX, tmb.sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, tmb.buffer.get());
								texSizeX[texSizeX.size() - 1] = tmb.sizeX;
								texSizeY[texSizeY.size() - 1] = tmb.sizeY;
							}
						}
					}
				}
				while (_findnext(hFile, &fileinfo) == 0);
				_findclose(hFile);
			}
			refresh = false;
		}
		enterbtn.enabled = chosenWorldName != "";
		deletebtn.enabled = chosenWorldName != "";
		if (backbtn.clicked) GUI::PopPage();
	}
	
	void WorldMenu::onRender()
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, windowheight - static_cast<int>((downp - 72) * stretch), windowwidth, static_cast<int>((downp - 72 - 48 + 1) * stretch));
		glTranslatef(0.0f, (float)-trs, 0.0f);
		for (int i = 0; i < worldcount; i++)
		{
			int xmin, xmax, ymin, ymax;
			xmin = midp - 250, xmax = midp + 250;
			ymin = 48 + i * 64, ymax = 48 + i * 64 + 60;
			if (thumbnails[i] == -1)
			{
				glDisable(GL_TEXTURE_2D);
				if (mouseon == i) glColor4f(0.5, 0.5, 0.5, GUI::FgA);
				else glColor4f(GUI::FgR, GUI::FgG, GUI::FgB, GUI::FgA);
				glBegin(GL_QUADS);
				UIVertex(midp - 250, 48 + i * 64);
				UIVertex(midp + 250, 48 + i * 64);
				UIVertex(midp + 250, 48 + i * 64 + 60);
				UIVertex(midp - 250, 48 + i * 64 + 60);
				glEnd();
			}
			else
			{
				bool marginOnSides;
				float w, h;
				//计算材质坐标，保持高宽比（按钮大小为500x60），有小学数学基础的人仔细想一想应该能懂QAQ
				if (texSizeX[i] * 60 / 500 < texSizeY[i])
				{
					marginOnSides = true;
					w = 1.0f, h = texSizeX[i] * 60 / 500.0f / texSizeY[i];
				}
				else
				{
					marginOnSides = false;
					w = texSizeY[i] * 500 / 60.0f / texSizeX[i];
					h = 1.0f;
				}
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, thumbnails[i]);
				if (mouseon == (int)i) glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				else glColor4f(0.8f, 0.8f, 0.8f, 0.9f);
				glBegin(GL_QUADS);
				glTexCoord2f(0.5f - w / 2, 0.5f + h / 2), UIVertex(midp - 250, 48 + i * 64);
				glTexCoord2f(0.5f + w / 2, 0.5f + h / 2), UIVertex(midp + 250, 48 + i * 64);
				glTexCoord2f(0.5f + w / 2, 0.5f - h / 2), UIVertex(midp + 250, 48 + i * 64 + 60);
				glTexCoord2f(0.5f - w / 2, 0.5f - h / 2), UIVertex(midp - 250, 48 + i * 64 + 60);
				glEnd();
			}
			glColor4f(GUI::FgR*0.9f, GUI::FgG*0.9f, GUI::FgB*0.9f, 0.9f);
			glDisable(GL_TEXTURE_2D);
			glLineWidth(1.0);
			glBegin(GL_LINE_LOOP);
			UIVertex(xmin, ymin);
			UIVertex(xmin, ymax);
			UIVertex(xmax, ymax);
			UIVertex(xmax, ymin);
			glEnd();
			if (selected == (int)i)
			{
				glLineWidth(2.0);
				glColor4f(0.0, 0.0, 0.0, 1.0);
				glBegin(GL_LINE_LOOP);
				UIVertex(midp - 250, 48 + i * 64);
				UIVertex(midp + 250, 48 + i * 64);
				UIVertex(midp + 250, 48 + i * 64 + 60);
				UIVertex(midp - 250, 48 + i * 64 + 60);
				glEnd();
			}
			TextRenderer::renderString(static_cast<int>(windowwidth / stretch - TextRenderer::getStrWidth(worldnames[i])) / 2, (140 + i * 128) / 2, worldnames[i]);
		}
		int i = worldcount;
		glDisable(GL_TEXTURE_2D);
		if (mouseon == i) glColor4f(0.5f, 0.5f, 0.5f, GUI::FgA);
		else glColor4f(GUI::FgR, GUI::FgG, GUI::FgB, GUI::FgA);
		glBegin(GL_QUADS);
		UIVertex(midp - 250, 48 + i * 64);
		UIVertex(midp + 250, 48 + i * 64);
		UIVertex(midp + 250, 48 + i * 64 + 60);
		UIVertex(midp - 250, 48 + i * 64 + 60);
		glEnd();
		glColor4f(GUI::FgR*0.9f, GUI::FgG*0.9f, GUI::FgB*0.9f, 0.9f);
		glDisable(GL_TEXTURE_2D);
		glLineWidth(1.0);
		glBegin(GL_LINE_LOOP);
		UIVertex(midp - 250, 48 + i * 64);
		UIVertex(midp + 250, 48 + i * 64);
		UIVertex(midp + 250, 48 + i * 64 + 60);
		UIVertex(midp - 250, 48 + i * 64 + 60);
		glEnd();
		TextRenderer::renderString(static_cast<int>(windowwidth / stretch - TextRenderer::getStrWidth(GetStrbyKey("NEWorld.worlds.new"))) / 2,
								   (140 + i * 128) / 2, GetStrbyKey("NEWorld.worlds.new"));
		glDisable(GL_SCISSOR_TEST);
	}
	
	void SoundMenu::onLoad()
	{
		title = GUI::label(GetStrbyKey("NEWorld.Sound.caption"), -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
		Musicbar = GUI::trackbar(GetStrbyKey("NEWorld.Sound.MusicGain"), 100, AudioSystem::BGMGain*300, -200, 201, 60, 84, 0.5, 0.5, 0.0, 0.0);
		SoundBar= GUI::trackbar(GetStrbyKey("NEWorld.Sound.SoundGain"), 100, AudioSystem::SoundGain*300, -200, 201, 90, 114, 0.5, 0.5, 0.0, 0.0);
		backbtn = GUI::button(GetStrbyKey("NEWorld.render.back"), -250, 250, -44, -20, 0.5, 0.5, 1.0, 1.0);
		registerControls(4, &title,&Musicbar,&SoundBar , &backbtn);
	}
	
	void SoundMenu::onUpdate()
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
	
	void OptionsMenu::onLoad()
	{
		title = GUI::label(GetStrbyKey("NEWorld.options.caption"), -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
		FOVyBar = GUI::trackbar("", 120, (int)(FOVyNormal - 1), -250, -10, 60, 84, 0.5, 0.5, 0.0, 0.0);
		mmsBar = GUI::trackbar("", 120, (int)(mousemove * 40 * 2 - 1), 10, 250, 60, 84, 0.5, 0.5, 0.0, 0.0);
		viewdistBar = GUI::trackbar("", 120, (ViewDistance - 2) * 4 - 1, -250, -10, 96, 120, 0.5, 0.5, 0.0, 0.0);
		rdstbtn = GUI::button(GetStrbyKey("NEWorld.options.rendermenu"), -250, -10, 204, 228, 0.5, 0.5, 0.0, 0.0);
		gistbtn = GUI::button(GetStrbyKey("NEWorld.options.guimenu"), 10, 250, 204, 228, 0.5, 0.5, 0.0, 0.0);
		langbtn = GUI::button(GetStrbyKey("NEWorld.options.languagemenu"), -250, -10, 240, 264, 0.5, 0.5, 0.0, 0.0);
		sounbtn = GUI::button(GetStrbyKey("NEWorld.options.soundmenu"), 10, 250, 240, 264, 0.5, 0.5, 0.0, 0.0);
		backbtn = GUI::button(GetStrbyKey("NEWorld.options.back"), -250, -10, -44, -20, 0.5, 0.5, 1.0, 1.0);
		savebtn = GUI::button(GetStrbyKey("NEWorld.options.save"), 10, 250, -44, -20, 0.5, 0.5, 1.0, 1.0);
		registerControls(10, &title, &FOVyBar, &mmsBar, &viewdistBar, &rdstbtn, &gistbtn, &backbtn, &savebtn, &langbtn,&sounbtn);
		//registerControls(8, &title, &FOVyBar, &mmsBar, &viewdistBar, &rdstbtn, &gistbtn, &backbtn, &savebtn);
	}
	
	void OptionsMenu::onUpdate()
	{
		FOVyNormal = (float)(FOVyBar.barpos + 1);
		mousemove = (mmsBar.barpos / 2 + 1) / 40.0f;
		ViewDistance = (viewdistBar.barpos + 1) / 4 + 2;
		if (rdstbtn.clicked) GUI::PushPage(new RenderOptionsMenu);
		if (gistbtn.clicked) GUI::PushPage(new GUIOptionsMenu);
		if (backbtn.clicked) GUI::PopPage();
		if (savebtn.clicked) SaveOptions();
		if (langbtn.clicked)
		{
			GUI::PushPage(new Language);
			title.text = GetStrbyKey("NEWorld.options.caption");
			rdstbtn.text = GetStrbyKey("NEWorld.options.rendermenu");
			gistbtn.text = GetStrbyKey("NEWorld.options.guimenu");
			langbtn.text = GetStrbyKey("NEWorld.options.languagemenu");
			backbtn.text = GetStrbyKey("NEWorld.options.back");
			savebtn.text = GetStrbyKey("NEWorld.options.save");
		}
		if (sounbtn.clicked) GUI::PushPage(new SoundMenu);

		AudioSystem::GUIUpdate();
		FOVyBar.text = GetStrbyKey("NEWorld.options.fov") + pack(FOVyNormal);
		mmsBar.text = GetStrbyKey("NEWorld.options.sensitivity") + pack(mousemove);
		viewdistBar.text = GetStrbyKey("NEWorld.options.distance") + pack(ViewDistance);
	}
	
	void GUIOptionsMenu::onLoad()
	{
		title = GUI::label(GetStrbyKey("NEWorld.gui.caption"), -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
		fontbtn = GUI::button("", -250, -10, 60, 84, 0.5, 0.5, 0.0, 0.0);
		blurbtn = GUI::button("", 10, 250, 60, 84, 0.5, 0.5, 0.0, 0.0);
		ppistretchbtn = GUI::button(GetStrbyKey("NEWorld.gui.stretch"), -250, -10, 96, 120, 0.5, 0.5, 0.0, 0.0);
		ppistat = GUI::label("", -250, 250, 120, 144, 0.5, 0.5, 0.0, 0.0);
		backbtn = GUI::button(GetStrbyKey("NEWorld.gui.back"), -250, 250, -44, -20, 0.5, 0.5, 1.0, 1.0);
		registerControls(6, &title, &fontbtn, &blurbtn, &ppistretchbtn, &ppistat, &backbtn);
		fontbtn.enabled = false;
	}

	void GUIOptionsMenu::onUpdate()
	{
		if (blurbtn.clicked)
			GUIScreenBlur = !GUIScreenBlur;
		if (ppistretchbtn.clicked)
		{
			if (stretch == 1.0)
				GUI::InitStretch();
			else
				GUI::EndStretch();
		}
		AudioSystem::GUIUpdate();
		if (backbtn.clicked)
			GUI::PopPage();
		fontbtn.text = GetStrbyKey("NEWorld.gui.unicode") + BoolYesNo(true);
		blurbtn.text = GetStrbyKey("NEWorld.gui.blur") + BoolEnabled(GUIScreenBlur);
		int vmc;
		const GLFWvidmode* m = glfwGetVideoModes(glfwGetPrimaryMonitor(), &vmc);
		ppistat.text = "phy:" + pack(GUI::nScreenWidth) + "x" + pack(GUI::nScreenHeight) +
					   " scr:" + pack(m[vmc - 1].width) + "x" + pack(m[vmc - 1].height) +
					   " win:" + pack(windowwidth) + "x" + pack(windowheight);
	}

	void Info::onLoad()
	{
		backbtn = GUI::button(GetStrbyKey("NEWorld.language.back"), -250, 250, -44, -20, 0.5, 0.5, 1.0, 1.0);
		registerControls(1, &backbtn);
	}

	void Info::onRender()
	{
		for (int i = 0; i < 13; ++i)
			TextRenderer::renderString(10, 10+20*i, Str[i]);
	}

	void Info::onUpdate()
	{
		AudioSystem::GUIUpdate();
		if (backbtn.clicked)
			GUI::PopPage();
	}
	
	void ShaderOptionsMenu::onLoad()
	{
		title = GUI::label(GetStrbyKey("NEWorld.shaders.caption"), -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
		enablebtn = GUI::button("", -250, -10, 60, 84, 0.5, 0.5, 0.0, 0.0);
		shadowresbar = GUI::trackbar("", 120, (int)(log2(Renderer::ShadowRes) - 10) * 40 - 1, 10, 250, 60, 84, 0.5, 0.5, 0.0, 0.0);
		shadowdistbar = GUI::trackbar("", 120, (Renderer::MaxShadowDist - 2) * 4 - 1, -250, -10, 96, 120, 0.5, 0.5, 0.0, 0.0);
		backbtn = GUI::button(GetStrbyKey("NEWorld.render.back"), -250, 250, -44, -20, 0.5, 0.5, 1.0, 1.0);
		registerControls(5, &title, &enablebtn, &shadowresbar, &shadowdistbar, &backbtn);
		Renderer::DestroyShaders();
		if (!Renderer::AdvancedRender) shadowresbar.enabled = shadowdistbar.enabled = false;
	}
	
	void ShaderOptionsMenu::onUpdate()
	{
		if (enablebtn.clicked)
		{
			Renderer::AdvancedRender = !Renderer::AdvancedRender;
			if (Renderer::AdvancedRender) shadowresbar.enabled = shadowdistbar.enabled = true;
			else shadowresbar.enabled = shadowdistbar.enabled = false;
		}
		Renderer::ShadowRes = (int)pow(2, (shadowresbar.barpos + 1) / 40 + 10);
		Renderer::MaxShadowDist = (shadowdistbar.barpos + 1) / 4 + 2;
		if (backbtn.clicked)
		{
			GUI::PopPage();
			if (Renderer::AdvancedRender) Renderer::InitShaders();
		}
		enablebtn.text = GetStrbyKey("NEWorld.shaders.enable") + BoolYesNo(Renderer::AdvancedRender);
		std::stringstream ss;
		ss << Renderer::ShadowRes;
		shadowresbar.text = GetStrbyKey("NEWorld.shaders.shadowres") + ss.str() + "x";
		ss.str("");
		ss << Renderer::MaxShadowDist;
		shadowdistbar.text = GetStrbyKey("NEWorld.shaders.distance") + ss.str();
		AudioSystem::GUIUpdate();
	}
}

GUI::Form* GUI::GetMain()
{
    return new Menus::MainMenu;
}
