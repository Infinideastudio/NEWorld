#pragma once
#include<AL\al.h>
#include<AL\alc.h>
#include<AL\CWaves.h>
#include<AL\aldlist.h>
#include<AL\AL-EFX.h>
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
    bool InitAL(ALCchar* DeviceName);
    void Updatelistener(ALfloat listenerPos[],ALfloat listenerVel[],ALfloat listenerOri[] );
    void Updatesource(ALuint Source,ALfloat sourcePos[], ALfloat sourceVel[]);
    bool load(char* WavFileName, ALuint* uiBuffer);
    ALuint Play(ALuint uiBuffer,bool loop,float gain, ALfloat sourcePos[], ALfloat sourceVel[]);
    void Stop(ALuint Source);
    void unload(ALuint uiBuffer);
    void ShutdownAL() {
        alcMakeContextCurrent(NULL);
        alcDestroyContext(Context);
        alcCloseDevice(Device);
    }
};

