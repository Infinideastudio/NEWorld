#include "GameSettings.h"
#include <sstream>
#include <fstream>
#include <map>
#include "Definitions.h"
#include "Renderer.h"
#include "Globalization.h"
#include "AudioSystem.h"

constexpr const char* SettingsFile = "./Configs/options.ini";

void GameSettings::loadOptions() {
	std::unordered_map<std::string, std::string> options;
	std::ifstream filein("./Configs/options.ini", std::ios::in);
	if (!filein.is_open()) return;
	std::string name, value;
	while (!filein.eof()) {
		filein >> name >> value;
		options[name] = value;
	}
	filein.close();
	loadOption(options, "Language", Globalization::Cur_Lang);
	loadOption(options, "FOV", FOVyNormal);
	loadOption(options, "RenderDistance", viewdistance);
	loadOption(options, "Sensitivity", mousemove);
	loadOption(options, "CloudWidth", cloudwidth);
	loadOption(options, "SmoothLighting", SmoothLighting);
	loadOption(options, "FancyGrass", NiceGrass);
	loadOption(options, "MultiSample", Multisample);
	loadOption(options, "AdvancedRender", Renderer::AdvancedRender);
	loadOption(options, "ShadowMapRes", Renderer::ShadowRes);
	loadOption(options, "ShadowDistance", Renderer::MaxShadowDist);
	loadOption(options, "VerticalSync", vsync);
	loadOption(options, "GainOfBGM", AudioSystem::BGMGain);
	loadOption(options, "GainOfSound", AudioSystem::SoundGain);
}

void GameSettings::saveOptions() {
	std::map<std::string, std::string> options;
	std::ofstream fileout("./Configs/options.ini", std::ios::out);
	if (!fileout.is_open()) return;
	saveOption(fileout, "Language", Globalization::Cur_Lang);
	saveOption(fileout, "FOV", FOVyNormal);
	saveOption(fileout, "RenderDistance", viewdistance);
	saveOption(fileout, "Sensitivity", mousemove);
	saveOption(fileout, "CloudWidth", cloudwidth);
	saveOption(fileout, "SmoothLighting", SmoothLighting);
	saveOption(fileout, "FancyGrass", NiceGrass);
	saveOption(fileout, "MultiSample", Multisample);
	saveOption(fileout, "AdvancedRender", Renderer::AdvancedRender);
	saveOption(fileout, "ShadowMapRes", Renderer::ShadowRes);
	saveOption(fileout, "ShadowDistance", Renderer::MaxShadowDist);
	saveOption(fileout, "VerticalSync", vsync);
	saveOption(fileout, "GainOfBGM", AudioSystem::BGMGain);
	saveOption(fileout, "GainOfSound", AudioSystem::SoundGain);
	fileout.close();
}
