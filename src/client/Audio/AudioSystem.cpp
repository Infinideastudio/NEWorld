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
#include "framework.h"
#include <memory>
namespace AudioSystem
{
//Gain
ALfloat BGMGain = 0.1f;//��������
ALfloat SoundGain = 0.17f;//��Ч
//Set
ALenum DopplerModel = AL_INVERSE_DISTANCE_CLAMPED;//����OpenAL�ľ���ģ��
ALfloat DopplerFactor = 1.0f;//����������
ALfloat SpeedOfSound = Air_SpeedOfSound;//����
//Update
bool FallBefore = false;//OnGround
bool DownWaterBefore = false;//InWater
int BGMNum = 0;
//Buffer
ALuint BGM[10];
ALuint Run = -1;
ALuint Click = -1;
ALuint Fall = -1;
ALuint BlockClick = -1;
ALuint DownWater = -1;
//Source
ALuint SBGM = -1;
ALuint SRun = -1;
ALuint SClick = -1;
ALuint SFall = -1;
ALuint SBlockClick = -1;
ALuint SDownWater = -1;
void Init()
{
    //��ʼ���豸
    //ALDeviceList *DL = getALDevice().GetALDeviceList();
    //getALDevice().init(DL->GetDeviceName(DL->GetDefaultDevice()));
    //delete DL;
    auto pDeviceList = std::make_unique<ALDeviceList>();
    getALDevice().init(pDeviceList->GetDeviceName(pDeviceList->GetDefaultDevice()));
    //ALFWInitOpenAL();
    //�������й���
    alEnable(AL_DOPPLER_FACTOR);
    alEnable(AL_DISTANCE_MODEL);
    alEnable(AL_SPEED_OF_SOUND);
    //��������
    char BGMName[256];
    for (int i = 0; i < 10; i++)
    {
        BGM[i] = -1;
    }
    for (int i = 0; i < 10; i++)
    {
        sprintf_s(BGMName, "Audio\\BGM%d.wav", i);
        if (getALDevice().load(BGMName, &BGM[BGMNum]))
        {
            BGMNum++;
        }
    }
    //����and�ܲ�����
    const char *wav_run = "Audio/Run.wav", *wav_click = "Audio/Click.wav",
                *wav_fall = "Audio/Fall.wav", *wav_blockclick = "Audio/BlockClick.wav",
                 *wav_downwater = "Audio/DownWater.wav";
    if (!getALDevice().load(wav_run, &Run))Run = -1;
    //��굥��
    if (!getALDevice().load(wav_click, &Click))Click = -1;
    //����
    if (!getALDevice().load(wav_fall, &Fall))Fall = -1;
    //���򷽿�
    if (!getALDevice().load(wav_blockclick, &BlockClick))BlockClick = -1;
    //��ˮ
    if (!getALDevice().load(wav_downwater, &DownWater))DownWater = -1;
    //����BGM
    if (BGMNum > 0)
    {
        int size = clock() % BGMNum;
        ALfloat Pos[] = { 0.0,0.0,0.0 };
        ALfloat Vel[] = { 0.0,0.0,0.0 };
        SBGM = getALDevice().play(BGM[size], false, BGMGain, Pos, Vel);
    }
}

void Update(ALfloat PlayerPos[3], bool BFall, bool BBlockClick, ALfloat BlockPos[3], int BRun, bool BDownWater)
{
    //����ȫ�ֳ���
    alDopplerFactor(DopplerFactor);
    alDistanceModel(DopplerModel);
    alSpeedOfSound(SpeedOfSound);
    //��������
    if (SBGM != -1)alSourcef(SBGM, AL_GAIN, BGMGain);
    if (SRun != -1)alSourcef(SRun, AL_GAIN, SoundGain);
    if (SClick != -1)alSourcef(SClick, AL_GAIN, SoundGain);
    if (SFall != -1)alSourcef(SFall, AL_GAIN, SoundGain);
    if (SBlockClick != -1)alSourcef(SBlockClick, AL_GAIN, SoundGain);
    if (SDownWater != -1)alSourcef(SDownWater, AL_GAIN, SoundGain);
    //���»���
    if (SBGM != -1)EFX::set(SBGM);
    if (SRun != -1)EFX::set(SRun);
    if (SClick != -1)EFX::set(SClick);
    if (SFall != -1)EFX::set(SFall);
    if (SBlockClick != -1)EFX::set(SBlockClick);
    if (SDownWater != -1)EFX::set(SDownWater);
    //����λ��
    PlayerPos[1] += 0.74;
    ALfloat Vel[] = { 0.0,0.0,0.0 };
    ALfloat Ori[] = { 0.0,0.0,-1.0, 0.0,1.0,0.0 };
    getALDevice().updateListener(PlayerPos, Vel, Ori);
    //����BGMλ��
    if (SBGM != -1)
    {
        ALint state;
        alGetSourcei(SBGM, AL_SOURCE_STATE, &state);
        if (state == AL_STOPPED)
        {
            getALDevice().stop(SBGM);
            if (BGMNum>0)
            {
                int size = clock() % BGMNum;
                ALfloat Pos[] = { 0.0,0.0,0.0 };
                ALfloat Vel[] = { 0.0,0.0,0.0 };
                SBGM = getALDevice().play(BGM[size], false, BGMGain, Pos, Vel);
            }
        }
        getALDevice().updateSource(SBGM, PlayerPos, Vel);
    }
    //����
    PlayerPos[1] -= 1.54;
    if (BFall != FallBefore)
    {
        if (BFall)
        {
            SFall = getALDevice().play(Fall, false, SoundGain, PlayerPos, Vel);
        }
        FallBefore = BFall;
    }
    else
    {
        if (SFall != -1)getALDevice().stop(SFall);
        SFall = -1;
    }
    //���򷽿�
    if (BBlockClick)
    {
        if (SBlockClick == -1)
        {
            SBlockClick = getALDevice().play(BlockClick, true, SoundGain, BlockPos, Vel);
        }
    }
    else
    {
        if (SBlockClick != -1)getALDevice().stop(SBlockClick);
        SBlockClick = -1;
    }
    //����
    if ((BRun != 0) && BFall)
    {
        if (SRun == -1)
        {
            SRun = getALDevice().play(Run, true, SoundGain, PlayerPos, Vel);
        }
        getALDevice().updateSource(SRun, PlayerPos, Vel);
        alSourcef(SRun, AL_PITCH, BRun*0.5f);
    }
    else
    {
        if (SRun != -1)getALDevice().stop(SRun);
        SRun = -1;
    }
    //��ˮ
    if (BDownWater != DownWaterBefore)
    {
        if (SDownWater == -1)SDownWater = getALDevice().play(DownWater, false, SoundGain, PlayerPos, Vel);
        DownWaterBefore = BDownWater;
    }
    else
    {
        if (SDownWater != -1)
        {
            ALint state;
            alGetSourcei(SDownWater, AL_SOURCE_STATE, &state);
            if (state == AL_STOPPED)
            {
                getALDevice().stop(SDownWater);
                SDownWater = -1;
            }
        }
    }
}
void ClickEvent()
{
    ALfloat Pos[] = { 0.0,0.0,0.0 };
    ALfloat Vel[] = { 0.0,0.0,0.0 };
    SClick = getALDevice().play(Click, false, SoundGain, Pos, Vel);
    //Sleep(50);
    clock_t c = clock();
    while (double(clock() - c) / double(CLOCKS_PER_SEC) < 0.05) {}
    getALDevice().stop(SClick);
    SClick = -1;
}
void GUIUpdate()
{
    SpeedOfSound = Air_SpeedOfSound;
    EFX::EAXprop = Generic;
    EFX::updateEAXprop();
    float Pos[] = { 0.0f,0.0f,0.0f };
    Update(Pos, false, false, Pos, false, false);
}
void UnInit()
{
    if (SBGM != -1)getALDevice().stop(SBGM);
    if (SRun != -1)getALDevice().stop(SRun);
    if (SClick != -1)getALDevice().stop(SClick);
    if (SFall != -1)getALDevice().stop(SFall);
    if (SBlockClick != -1)getALDevice().stop(SBlockClick);
    if (SDownWater != -1)getALDevice().stop(SDownWater);

    for (size_t i = 0; i < 10; i++)
    {
        if (BGM[i] != -1)getALDevice().unload(BGM[i]);
    }
    if (Run != -1)getALDevice().unload(Run);
    if (Click != -1)getALDevice().unload(Click);
    if (Fall != -1)getALDevice().unload(Fall);
    if (BlockClick != -1)getALDevice().unload(BlockClick);
    if (DownWater != -1)getALDevice().unload(DownWater);

    getALDevice().clean();
}
}