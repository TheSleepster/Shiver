/* date = June 21st 2024 3:23 am */

#ifndef _SHIVER__AUDIO_ENGINE_H
#define _SHIVER__AUDIO_ENGINE_H

#include "FMOD/fmod.h"
#include "FMOD/fmod_common.h"

// FMOD STUDIO
#include "FMOD/fmod_studio.h"
#include "FMOD/fmod_studio_common.h"

struct fmod_sound_event
{
    const char *SoundFXPath;
    
    FMOD_STUDIO_EVENTDESCRIPTION *EventDesc;
    FMOD_STUDIO_EVENTINSTANCE *EventInstance;
};

enum audiosfx
{
    SFX_TEST,
    SFX_COUNT
};

struct fmod_sound_subsystem_data
{
    FMOD_STUDIO_SYSTEM *StudioSystem;
    FMOD_SYSTEM *CoreSystem;
    
    FMOD_STUDIO_BANK *MasterBank;
    FMOD_STUDIO_BANK *MasterStringsBank;
    
    const char *MasterBankFilepath;
    const char *StringsBankFilepath;
    
    FILETIME MasterBankLastWriteTime;
    FILETIME StringsBankLastWriteTime;
    
    fmod_sound_event SoundFX[255];
};

internal inline void
sh_FMODPlaySoundSFX(fmod_sound_event SoundFX)
{
    FMOD_STUDIO_PLAYBACK_STATE FMODSoundState;
    
    FMOD_Studio_EventDescription_CreateInstance(SoundFX.EventDesc, &SoundFX.EventInstance);
    FMOD_Studio_EventInstance_GetPlaybackState(SoundFX.EventInstance, &FMODSoundState);
    if(FMODSoundState == FMOD_STUDIO_PLAYBACK_STOPPED)
    {
        FMOD_Studio_EventInstance_Start(SoundFX.EventInstance);
        FMOD_Studio_EventInstance_Release(SoundFX.EventInstance);
    }
}

#endif //_SHIVER__AUDIO_ENGINE_H
