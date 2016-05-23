#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include "aldlist.h"
#include "AL-EFX.h"

class ALDevice
{
private:
    ALCdevice *Device = nullptr;
    ALCcontext *Context = nullptr;

public:
    static const ALuint INVALID_BUFFER = static_cast<ALuint>(-1);
    bool init();
    void updateListener(ALfloat listenerPos[], ALfloat listenerVel[], ALfloat listenerOri[]);
    void updateSource(ALuint Source,ALfloat sourcePos[], ALfloat sourceVel[]);
    ALuint ALDevice::load(string FileName);
    ALuint play(ALuint uiBuffer,bool loop,float gain, ALfloat sourcePos[], ALfloat sourceVel[]);
    void stop(ALuint Source);
    void unload(ALuint uiBuffer);
    void clean();

};

inline ALDevice& getALDevice()
{
    static ALDevice device;
    return device;
}
