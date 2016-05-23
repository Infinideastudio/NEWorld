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
#include "ALDevice.h"
#include "AudioSystem.h"
#include <memory>
#include <ctime>
namespace AudioSystem
{
//Gain
ALfloat BGMGain = 0.1f;//背景音乐
ALfloat SoundGain = 0.17f;//音效
//Set
ALenum DopplerModel = AL_INVERSE_DISTANCE_CLAMPED;//设置OpenAL的距离模型
ALfloat DopplerFactor = 1.0f;//多普勒因子
ALfloat SpeedOfSound = Air_SpeedOfSound;//声速
//Update
bool FallBefore = false;//OnGround
bool DownWaterBefore = false;//InWater
int BGMNum = 0;

std::map<string, Sound> sounds;

void init()
{
    getALDevice().init();
    //加载音乐
    for (int i = 0; i < 10; i++)
    {
        string strID = std::to_string(i);
        if (load("BGM" + strID, "Audio\\BGM" + strID + ".wav"))
            BGMNum++;
        else
            break;
    }
    load("Run", "Audio/Run.wav");
    load("Click", "Audio/Click.wav");
    load("Fall", "Audio/Fall.wav");
    load("BlockClick", "Audio/BlockClick.wav");
    load("DownWater", "Audio/DownWater.wav");
    //播放BGM
    if (BGMNum > 0)
        play("BGM" + std::to_string(clock() % BGMNum), false, BGMGain, { 0.0,0.0,0.0 });
}
void unload()
{
    for (auto& s : sounds)
    {
        getALDevice().stop(s.second.source);
        getALDevice().unload(s.second.buffer);
    }
    sounds.clear();
    getALDevice().clean();
}
void play(string name, bool loop, float gain, Vec3d sourcePos)
{
    Sound& s = sounds[name];
    //设置全局常量
    alDopplerFactor(DopplerFactor);
    alDistanceModel(DopplerModel);
    alSpeedOfSound(SpeedOfSound);
    for (auto& fs : sounds)
    {
        if (fs.second.source == Sound::INVALID_SOURCE) continue;
        alSourcef(fs.second.source, AL_GAIN, BGMGain);
        EFX::set(fs.second.source);
    }
    ALfloat pos[3] = { (ALfloat)sourcePos.x,(ALfloat)sourcePos.y,(ALfloat)sourcePos.z };
    ALfloat Vel[] = { 0,0,0 };
    s.source = getALDevice().play(s.buffer, loop, gain, pos, Vel);
}
void stop(string name)
{
    Sound& s = sounds[name];
    if(s.source!=Sound::INVALID_SOURCE)
        getALDevice().stop(s.source);
}
bool load(string name, string path)
{
    Sound s = Sound(getALDevice().load(path));
    if (s.buffer == ALDevice::INVALID_BUFFER) return false;
    sounds.insert(std::make_pair(name, s));
    return true;
}

}
