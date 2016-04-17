#pragma once
#include <AL/Framework.h>
#ifdef NEWORLD_TARGET_WINDOWS
#include <AL/EFX-Util.h>
#endif

#define Generic REVERB_PRESET_GENERIC;
#define Plain REVERB_PRESET_PLAIN;
#define UnderWater REVERB_PRESET_UNDERWATER;

namespace EFX
{
    extern EAXREVERBPROPERTIES EAXprop;
    bool Init();
    void set(ALuint Source);
    bool UpdateEAXprop();
    ALboolean CreateAuxEffectSlot(ALuint *puiAuxEffectSlot);
    ALboolean CreateEffect(ALuint *puiEffect, ALenum eEffectType);
    ALboolean SetEFXEAXReverbProperties(EFXEAXREVERBPROPERTIES *pEFXEAXReverb, ALuint uiEffect);
}
