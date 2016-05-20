#pragma once
#include "Framework.h"
#ifdef NEWORLD_TARGET_WINDOWS
#include <AL/EFX-Util.h>

#define Generic REVERB_PRESET_GENERIC;
#define Plain REVERB_PRESET_PLAIN;
#define UnderWater REVERB_PRESET_UNDERWATER;

namespace EFX
{
extern EAXREVERBPROPERTIES EAXprop;
bool init();
void set(ALuint Source);
bool updateEAXprop();
ALboolean createAuxEffectSlot(ALuint *puiAuxEffectSlot);
ALboolean createEffect(ALuint *puiEffect, ALenum eEffectType);
ALboolean setEFXEAXReverbProperties(EFXEAXREVERBPROPERTIES *pEFXEAXReverb, ALuint uiEffect);
}
#endif
