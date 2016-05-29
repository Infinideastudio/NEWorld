#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include "aldlist.h"
#include "AL-EFX.h"

class ALDevice
{
public:
    static constexpr ALuint INVALID_BUFFER = static_cast<ALuint>(-1);
    bool init()
    {
        unique_ptr<ALDeviceList> pDeviceList(new ALDeviceList);
        Device = alcOpenDevice(pDeviceList->GetDeviceName(pDeviceList->GetDefaultDevice()));
        if (!Device)
            return false;
        Context = alcCreateContext(Device, NULL);
        if (!Context)
        {
            alcCloseDevice(Device);
            return false;
        }
        alcMakeContextCurrent(Context);
#ifdef NEWORLD_TARGET_WINDOWS
        EFX::init();
#endif
        alEnable(AL_DOPPLER_FACTOR);
        alEnable(AL_DISTANCE_MODEL);
        alEnable(AL_SPEED_OF_SOUND);
        return true;
    }
    void updateListener(ALfloat listenerPos[], ALfloat listenerVel[], ALfloat listenerOri[])
    {
        alListenerfv(AL_POSITION, listenerPos);
        alListenerfv(AL_VELOCITY, listenerVel);
        alListenerfv(AL_ORIENTATION, listenerOri);
    }
    void updateSource(ALuint Source, ALfloat sourcePos[], ALfloat sourceVel[])
    {
        alSourcefv(Source, AL_POSITION, sourcePos);
        alSourcefv(Source, AL_VELOCITY, sourceVel);
    }
    ALuint ALDevice::load(const string& FileName);
    ALuint play(ALuint uiBuffer,bool loop,float gain, ALfloat sourcePos[], ALfloat sourceVel[]);
    void stop(ALuint Source)
    {
        alSourceStop(Source);
        alDeleteSources(1, &Source);
    }
    void unload(ALuint uiBuffer)
    {
        alDeleteBuffers(1, &uiBuffer);
    }
    void clean()
    {
        alcMakeContextCurrent(0);
        alcDestroyContext(Context);
        alcCloseDevice(Device);
    }

private:
    ALCdevice *Device = nullptr;
    ALCcontext *Context = nullptr;
};
extern ALDevice device;
