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
#include "..\..\shared\vec3.h"
#include <map>
namespace AudioSystem
{
struct Sound
{
    ALuint buffer, source;
    static constexpr ALuint INVALID_SOURCE = static_cast<ALuint>(-1);
    explicit Sound(ALuint _buffer) :buffer(_buffer), source(INVALID_SOURCE) {}
};
extern ALfloat BGMGain;//背景音乐
extern ALfloat SoundGain;//音效

extern ALenum DopplerModel;//距离模型
extern ALfloat DopplerFactor;//多普勒因子
extern ALfloat SpeedOfSound;//声速
constexpr ALfloat Air_SpeedOfSound = 343.3f, Water_SpeedOfSound = 1473.0f;

extern std::map<string, Sound> sounds;

void init();
void unload();

void play(const string& name, bool loop, float gain, const Vec3d& sourcePos);
void stop(const string& name);
bool load(const string& name, const string& path);

}

#endif
