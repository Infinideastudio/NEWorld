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
#include <memory>
#include <ctime>
#include <cassert>
#include "ALDevice.h"
#include "AudioSystem.h"
namespace AudioSystem
{
ALfloat BGMGain = 0.1f, SoundGain = 0.17f,DopplerFactor = 1.0f, SpeedOfSound = Air_SpeedOfSound;
ALenum DopplerModel = AL_INVERSE_DISTANCE_CLAMPED;
bool FallBefore = false, DownWaterBefore = false;
int BGMNum = 0;

std::map<string, Sound> sounds;

void init()
{
    device.init();
    //加载音乐
    for (int i = 0;; i++)
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
    if (BGMNum > 0)
        play("BGM" + std::to_string(clock() % BGMNum), false, BGMGain, { 0.0,0.0,0.0 });
}
void unload()
{
    for (auto s : sounds)
    {
        device.stop(s.second.source);
        device.unload(s.second.buffer);
    }
    sounds.clear();
    device.clean();
}
void play(const string& name, bool loop, float gain, const Vec3d& sourcePos)
{
    Sound& s = sounds[name];
    //设置全局常量
    alDopplerFactor(DopplerFactor);
    alDistanceModel(DopplerModel);
    alSpeedOfSound(SpeedOfSound);
    for (auto fs : sounds)
    {
        assert(fs.second.source != Sound::INVALID_SOURCE);
        alSourcef(fs.second.source, AL_GAIN, BGMGain);
        EFX::set(fs.second.source);
    }
    ALfloat pos[3] = { (ALfloat)sourcePos.x,(ALfloat)sourcePos.y,(ALfloat)sourcePos.z };
    ALfloat Vel[] = { 0,0,0 };
    s.source = device.play(s.buffer, loop, gain, pos, Vel);
}
void stop(const string& name)
{
    ALuint s = sounds[name].source;
    assert(s != Sound::INVALID_SOURCE);
    device.stop(s);
}
bool load(const string& name, const string& path)
{
    Sound s(device.load(path));
    assert(s.buffer == ALDevice::INVALID_BUFFER);
    sounds.insert(std::make_pair(name, s));
    return true;
}

}
