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

struct Langinfo
{
    string Symbol, EngSymbol, Name;
    GUI::button * Button;
};

void SaveOptions();

string Str[] =
{
    "NEWorld Main Version:" + MAJOR_VERSION + MINOR_VERSION + EXT_VERSION ,
    "Copyright (c) 2016 Infinideastudio under the Public Domain" ,
    "Welcome to develope with us!",
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
		for (unsigned int i = 0; i != s.size(); i++)
			if (s[i] == '.') ret++;
		return ret;
	}

	class MultiplayerMenu :public GUI::Form
	{
	private:
		GUI::label title;
		GUI::textbox serveriptb;
		GUI::button runbtn, okbtn, backbtn;
		void onLoad()
		{
			title = GUI::label("==============<  多 人 游 戏  >==============", -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
			serveriptb = GUI::textbox("输入服务器IP", -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
			runbtn = GUI::button("运行服务器（开服）", -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
			okbtn = GUI::button("确定", -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
			backbtn = GUI::button("<< 返回", -225, 225, 20, 36, 0.5, 0.5, 0.0, 0.0);
			inputstr = "";
			okbtn.enabled = false;
			registerControls(4, &title, &serveriptb, &runbtn, &okbtn, &backbtn);
		}
		void onUpdate()
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
	};
	void multiplayermenu()
	{
		GUI::PushPage(new MultiplayerMenu);
	}
	
	class GameMenu :public GUI::Form
	{
	private:
		GUI::label title;
		GUI::button resumebtn, exitbtn;

		void onLoad()
		{
			glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			title = GUI::label(GetStrbyKey("NEWorld.pause.caption"), -225, 225, 0, 16, 0.5, 0.5, 0.25, 0.25);
			resumebtn = GUI::button(GetStrbyKey("NEWorld.pause.continue"), -200, 200, -35, -3, 0.5, 0.5, 0.5, 0.5);
			exitbtn = GUI::button(GetStrbyKey("NEWorld.pause.back"), -200, 200, 3, 35, 0.5, 0.5, 0.5, 0.5);
			registerControls(3, &title, &resumebtn, &exitbtn);
		}

		void onUpdate()
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
	};

	void gamemenu()
	{
		GUI::PushPage(new GameMenu);
	}
	
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
	
	class MainMenu :public GUI::Form
	{
	private:
		GUI::imagebox title;
		GUI::button startbtn, optionsbtn, quitbtn, info;
		void onLoad()
		{
			title = GUI::imagebox(0.0f, 1.0f, 0.5f, 1.0f, tex_title, -256, 256, 20, 276, 0.5, 0.5, 0.0, 0.0);
			startbtn = GUI::button(GetStrbyKey("NEWorld.main.start"), -200, 200, 280, 312, 0.5, 0.5, 0.0, 0.0);
			optionsbtn = GUI::button(GetStrbyKey("NEWorld.main.options"), -200, -3, 318, 352, 0.5, 0.5, 0.0, 0.0);
			quitbtn = GUI::button(GetStrbyKey("NEWorld.main.exit"), 3, 200, 318, 352, 0.5, 0.5, 0.0, 0.0);
			info = GUI::button("i", 210, 250, 318, 352, 0.5, 0.5, 0.0, 0.0);
			registerControls(5, &title, &startbtn, &optionsbtn, &info, &quitbtn);
		}
		void onUpdate()
		{
			AudioSystem::GUIUpdate();
			if (startbtn.clicked) worldmenu();
			if (gamebegin) GUI::PopPage();
			if (optionsbtn.clicked)
			{
				options();
				startbtn.text = GetStrbyKey("NEWorld.main.start");
				optionsbtn.text = GetStrbyKey("NEWorld.main.options");
				quitbtn.text = GetStrbyKey("NEWorld.main.exit");
			}
			if (info.clicked) Information();
			if (quitbtn.clicked) exit(0);
		}
	};
	
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
				for (vector<GUI::control*>::iterator iter = children.begin(); iter != children.end(); )
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
	
	class RenderOptionsMenu :public GUI::Form
	{
	private:
		GUI::label title;
		GUI::button smoothlightingbtn, fancygrassbtn, mergefacebtn, shaderbtn, vsyncbtn, backbtn;
		GUI::trackbar msaabar;
		void onLoad()
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
		void onUpdate()
		{
			glfwGetMonitorPhysicalSize(glfwGetPrimaryMonitor(), &GUI::nScreenWidth,
									   &GUI::nScreenHeight);
			if (smoothlightingbtn.clicked) SmoothLighting = !SmoothLighting;
			if (fancygrassbtn.clicked) NiceGrass = !NiceGrass;
			if (mergefacebtn.clicked) MergeFace = !MergeFace;
			if (msaabar.barpos == 0) Multisample = 0;
			else Multisample = 1 << ((msaabar.barpos + 1) / 30 + 1);
			if (shaderbtn.clicked) Shaderoptions();
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
	};
	void Renderoptions()
	{
		GUI::PushPage(new RenderOptionsMenu);
	}
	
	class WorldMenu :public GUI::Form
	{
	private:
		int leftp = static_cast<int>(windowwidth / 2.0 / stretch  - 200);
		int midp = static_cast<int>(windowwidth / 2.0 / stretch);
		int rightp = static_cast<int>(windowwidth / 2.0 / stretch+ 200);
		int downp = static_cast<int>(windowheight / stretch - 20);
		bool refresh = true;
		int selected = 0, mouseon;
		int worldcount;
		string chosenWorldName;
		vector<string> worldnames;
		vector<TextureID> thumbnails, texSizeX, texSizeY;
		int trs = 0;
		GUI::label title;
		GUI::vscroll vscroll;
		GUI::button enterbtn, deletebtn, backbtn;
		void onLoad()
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
		void onUpdate()
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
						createworldmenu();
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
		void onRender()
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
	};
	void worldmenu()
	{
		GUI::PushPage(new WorldMenu);
	}
	
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
	
	class OptionsMenu :public GUI::Form
	{
	private:
		GUI::label title;
		GUI::trackbar FOVyBar, mmsBar, viewdistBar;
		GUI::button rdstbtn, langbtn, gistbtn, backbtn, savebtn, sounbtn;
		//GUI::button rdstbtn, gistbtn, backbtn, savebtn;
		void onLoad()
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
		void onUpdate()
		{
			FOVyNormal = (float)(FOVyBar.barpos + 1);
			mousemove = (mmsBar.barpos / 2 + 1) / 40.0f;
			ViewDistance = (viewdistBar.barpos + 1) / 4 + 2;
			if (rdstbtn.clicked) Renderoptions();
			if (gistbtn.clicked) GUIoptions();
			if (backbtn.clicked) GUI::PopPage();
			if (savebtn.clicked) SaveOptions();
			if (langbtn.clicked)
			{
				languagemenu();
				title.text = GetStrbyKey("NEWorld.options.caption");
				rdstbtn.text = GetStrbyKey("NEWorld.options.rendermenu");
				gistbtn.text = GetStrbyKey("NEWorld.options.guimenu");
				langbtn.text = GetStrbyKey("NEWorld.options.languagemenu");
				backbtn.text = GetStrbyKey("NEWorld.options.back");
				savebtn.text = GetStrbyKey("NEWorld.options.save");
			}
			if (sounbtn.clicked)Soundmenu();

			AudioSystem::GUIUpdate();
			FOVyBar.text = strWithVar(GetStrbyKey("NEWorld.options.fov"), FOVyNormal);
			mmsBar.text = strWithVar(GetStrbyKey("NEWorld.options.sensitivity"), mousemove);
			viewdistBar.text = strWithVar(GetStrbyKey("NEWorld.options.distance"), ViewDistance);
		}
	};
	void options()
	{
		GUI::PushPage(new OptionsMenu);
	}
	
	class GUIOptionsMenu :public GUI::Form
	{
	private:
		GUI::label title, ppistat;
		GUI::button fontbtn, blurbtn, ppistretchbtn, backbtn;

		void onLoad()
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

		void onUpdate()
		{
			/*if (fontbtn.clicked)
				TextRenderer::UseUnicodeASCIIFont = !TextRenderer::UseUnicodeASCIIFont;*/
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
			//fontbtn.text = GetStrbyKey("NEWorld.gui.unicode") + BoolYesNo(TextRenderer::UseUnicodeASCIIFont);
			fontbtn.text = GetStrbyKey("NEWorld.gui.unicode") + BoolYesNo(true);
			blurbtn.text = GetStrbyKey("NEWorld.gui.blur") + BoolEnabled(GUIScreenBlur);
			int vmc;
			const GLFWvidmode* m = glfwGetVideoModes(glfwGetPrimaryMonitor(), &vmc);
			ppistat.text = "phy:" + Var2Str(GUI::nScreenWidth) + "x" + Var2Str(GUI::nScreenHeight) +
						   " scr:" + Var2Str(m[vmc - 1].width) + "x" + Var2Str(m[vmc - 1].height) +
						   " win:" + Var2Str(windowwidth) + "x" + Var2Str(windowheight);
		}
	};

	void GUIoptions()
	{
		GUI::PushPage(new GUIOptionsMenu);
	}
	
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
	
	class ShaderOptionsMenu :public GUI::Form
	{
	private:
		GUI::label title;
		GUI::button enablebtn, backbtn;
		GUI::trackbar shadowresbar, shadowdistbar;
		void onLoad()
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
		void onUpdate()
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
	};
	void Shaderoptions()
	{
		GUI::PushPage(new ShaderOptionsMenu);
	}
}

GUI::Form* GUI::GetMain()
{
    return new Menus::MainMenu;
}