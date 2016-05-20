#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include "aldlist.h"
#include "AL-EFX.h"

class ALDevice
{
public:
    ALCdevice *Device = 0;
    ALCcontext *Context = 0;
    ALDevice()
    {
    }

    ~ALDevice()
    {
    }
    ALDeviceList* GetALDeviceList();
    bool init(ALCchar* DeviceName);
    void updateListener(ALfloat listenerPos[],ALfloat listenerVel[],ALfloat listenerOri[] );
    void updateSource(ALuint Source,ALfloat sourcePos[], ALfloat sourceVel[]);
    bool load(const char* FileName, ALuint* uiBuffer);
    ALuint play(ALuint uiBuffer,bool loop,float gain, ALfloat sourcePos[], ALfloat sourceVel[]);
    void stop(ALuint Source);
    void unload(ALuint uiBuffer);
    void clean();
};

