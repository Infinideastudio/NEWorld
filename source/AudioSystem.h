#pragma once
#include<AL\ALDevice.h>
namespace AudioSystem {
	extern ALDevice Device;
	//Gain
	extern ALfloat BGMGain;//背景音乐
	extern ALfloat SoundGain;//音效
	//Set
	extern ALenum DopplerModel;//设置OpenAL的距离模型
	extern ALfloat DopplerFactor;//多普勒因子
	extern ALfloat SpeedOfSound;//声速
	const ALfloat Air_SpeedOfSound = 343.3f;
	const ALfloat Water_SpeedOfSound = 1473.0f;
	void Init();
	void Update(ALfloat PlayerPos[3],bool BFall, bool BBlockClick, ALfloat BlockPos[3], int Run, bool BDownWater);
	void ClickEvent();
	void GUIUpdate();
	void UnInit();
}