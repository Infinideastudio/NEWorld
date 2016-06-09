/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include "ALDevice.h"
#include <ctime>
namespace AudioSystem
{

//Gain
extern ALfloat BGMGain;//��������
extern ALfloat SoundGain;//��Ч
//Set
extern ALenum DopplerModel;//����OpenAL�ľ���ģ��
extern ALfloat DopplerFactor;//����������
extern ALfloat SpeedOfSound;//����
const ALfloat Air_SpeedOfSound = 343.3f;
const ALfloat Water_SpeedOfSound = 1473.0f;
void Init();
void Update(ALfloat PlayerPos[3], bool BFall, bool BBlockClick, ALfloat BlockPos[3], int Run, bool BDownWater);
void ClickEvent();
void GUIUpdate();
void UnInit();

}

#endif