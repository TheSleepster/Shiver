/* date = June 21st 2024 3:23 am */

#ifndef _SHIVER__AUDIO_ENGINE_H
#define _SHIVER__AUDIO_ENGINE_H

#if 0
#include "../data/deps/FMOD/fmod.h"
#include "../data/deps/FMOD/fmod_common.h"

// FMOD STUDIO
#include "../data/deps/FMOD/fmod_studio.h"
#include "../data/deps/FMOD/fmod_studio_common.h"

struct fmod_sound_event
{
    const char *SoundFXPath;

    FMOD_STUDIO_EVENTDESCRIPTION *EventDesc;
    FMOD_STUDIO_EVENTINSTANCE *EventInstance;
};

enum audiosfx
{
    TEST_SFX,
    TEST_BOOP,
    TEST_MUSIC,
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


// NOTE(Sleepster): Determine whether or not this is something I want to keep like this.
//                  It breaks the barrier betwen "Game/Engine" functionality
internal inline void
sh_FMODPlaySoundFX(fmod_sound_event SoundFX)
{
    FMOD_Studio_EventDescription_CreateInstance(SoundFX.EventDesc, &SoundFX.EventInstance);
    FMOD_Studio_EventInstance_Start(SoundFX.EventInstance);
    FMOD_Studio_EventInstance_Release(SoundFX.EventInstance);
}

// TODO(Sleepster): Impliment this
internal inline void
sh_FMODPlayLoopedSoundFX(fmod_sound_event SoundFX)
{
    FMOD_Studio_EventDescription_CreateInstance(SoundFX.EventDesc, &SoundFX.EventInstance);
    FMOD_Studio_EventInstance_Start(SoundFX.EventInstance);
    FMOD_Studio_EventInstance_Release(SoundFX.EventInstance);
}
#endif

#define MINIAUDIO_IMPLEMENTATION
#include "../data/deps/MiniAudio/miniaudio.h"

struct shiver_audio_engine
{
    ma_context sh_AudioContext;
    ma_context_config sh_AudioContextConfig;

    ma_device sh_AudioOutputDevice;
    ma_device_config sh_AudioDeviceConfig;

    ma_decoder sh_AudioDecoder;

    ma_resource_manager sh_AudioManager;
    ma_resource_manager_config sh_AudioManagerConfig;
    ma_resource_manager_data_source sh_AudioManagerSource;
};

#endif //_SHIVER__AUDIO_ENGINE_H
