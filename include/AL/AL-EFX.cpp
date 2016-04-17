#include "AL-EFX.h"
#ifdef NEWORLD_TARGET_WINDOWS
namespace EFX
{
    ALuint uiEffectSlot, uiEffect;
    EFXEAXREVERBPROPERTIES efxReverb;
    EAXREVERBPROPERTIES EAXprop = Generic;
    ALboolean CreateEffect(ALuint *puiEffect, ALenum eEffectType);
    ALboolean CreateAuxEffectSlot(ALuint *puiAuxEffectSlot);
    ALboolean SetEFXEAXReverbProperties(EFXEAXREVERBPROPERTIES *pEFXEAXReverb, ALuint uiEffect);

    bool UpdateEAXprop();

    bool Init()
    {
        if (ALFWIsEFXSupported() && CreateAuxEffectSlot(&uiEffectSlot) && CreateEffect(&uiEffect, AL_EFFECT_EAXREVERB))
            return UpdateEAXprop();
        return false;

    }
    
    void set(ALuint Source)
    {
        alSource3i(Source, AL_AUXILIARY_SEND_FILTER, uiEffectSlot, 0, AL_FILTER_NULL);
    }

    bool UpdateEAXprop()
    {
        ConvertReverbParameters(&EAXprop, &efxReverb);
        if (SetEFXEAXReverbProperties(&efxReverb, uiEffect))
        {
            alAuxiliaryEffectSloti(uiEffectSlot, AL_EFFECTSLOT_EFFECT, uiEffect);
            return true;
        }
        else
        {
            return false;
        }
    }

    ALboolean CreateAuxEffectSlot(ALuint *puiAuxEffectSlot)
    {
        ALboolean bReturn = AL_FALSE;

        // Clear AL Error state
        alGetError();

        // Generate an Auxiliary Effect Slot
        alGenAuxiliaryEffectSlots(1, puiAuxEffectSlot);
        if (alGetError() == AL_NO_ERROR)
            bReturn = AL_TRUE;

        return bReturn;
    }

    ALboolean CreateEffect(ALuint *puiEffect, ALenum eEffectType)
    {
        ALboolean bReturn = AL_FALSE;

        if (puiEffect)
        {
            // Clear AL Error State
            alGetError();

            // Generate an Effect
            alGenEffects(1, puiEffect);
            if (alGetError() == AL_NO_ERROR)
            {
                // Set the Effect Type
                alEffecti(*puiEffect, AL_EFFECT_TYPE, eEffectType);
                if (alGetError() == AL_NO_ERROR)
                    bReturn = AL_TRUE;
                else
                    alDeleteEffects(1, puiEffect);
            }
        }
        return bReturn;
    }

    ALboolean SetEFXEAXReverbProperties(EFXEAXREVERBPROPERTIES *pEFXEAXReverb, ALuint uiEffect)
    {
        ALboolean bReturn = AL_FALSE;

        if (pEFXEAXReverb)
        {
            // Clear AL Error code
            alGetError();

            alEffectf(uiEffect, AL_EAXREVERB_DENSITY, pEFXEAXReverb->flDensity);
            alEffectf(uiEffect, AL_EAXREVERB_DIFFUSION, pEFXEAXReverb->flDiffusion);
            alEffectf(uiEffect, AL_EAXREVERB_GAIN, pEFXEAXReverb->flGain);
            alEffectf(uiEffect, AL_EAXREVERB_GAINHF, pEFXEAXReverb->flGainHF);
            alEffectf(uiEffect, AL_EAXREVERB_GAINLF, pEFXEAXReverb->flGainLF);
            alEffectf(uiEffect, AL_EAXREVERB_DECAY_TIME, pEFXEAXReverb->flDecayTime);
            alEffectf(uiEffect, AL_EAXREVERB_DECAY_HFRATIO, pEFXEAXReverb->flDecayHFRatio);
            alEffectf(uiEffect, AL_EAXREVERB_DECAY_LFRATIO, pEFXEAXReverb->flDecayLFRatio);
            alEffectf(uiEffect, AL_EAXREVERB_REFLECTIONS_GAIN, pEFXEAXReverb->flReflectionsGain);
            alEffectf(uiEffect, AL_EAXREVERB_REFLECTIONS_DELAY, pEFXEAXReverb->flReflectionsDelay);
            alEffectfv(uiEffect, AL_EAXREVERB_REFLECTIONS_PAN, pEFXEAXReverb->flReflectionsPan);
            alEffectf(uiEffect, AL_EAXREVERB_LATE_REVERB_GAIN, pEFXEAXReverb->flLateReverbGain);
            alEffectf(uiEffect, AL_EAXREVERB_LATE_REVERB_DELAY, pEFXEAXReverb->flLateReverbDelay);
            alEffectfv(uiEffect, AL_EAXREVERB_LATE_REVERB_PAN, pEFXEAXReverb->flLateReverbPan);
            alEffectf(uiEffect, AL_EAXREVERB_ECHO_TIME, pEFXEAXReverb->flEchoTime);
            alEffectf(uiEffect, AL_EAXREVERB_ECHO_DEPTH, pEFXEAXReverb->flEchoDepth);
            alEffectf(uiEffect, AL_EAXREVERB_MODULATION_TIME, pEFXEAXReverb->flModulationTime);
            alEffectf(uiEffect, AL_EAXREVERB_MODULATION_DEPTH, pEFXEAXReverb->flModulationDepth);
            alEffectf(uiEffect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, pEFXEAXReverb->flAirAbsorptionGainHF);
            alEffectf(uiEffect, AL_EAXREVERB_HFREFERENCE, pEFXEAXReverb->flHFReference);
            alEffectf(uiEffect, AL_EAXREVERB_LFREFERENCE, pEFXEAXReverb->flLFReference);
            alEffectf(uiEffect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, pEFXEAXReverb->flRoomRolloffFactor);
            alEffecti(uiEffect, AL_EAXREVERB_DECAY_HFLIMIT, pEFXEAXReverb->iDecayHFLimit);

            if (alGetError() == AL_NO_ERROR)
                bReturn = AL_TRUE;
        }

        return bReturn;
    }
}
#endif
