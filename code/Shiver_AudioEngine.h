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

#include "Shiver_Globals.h"

#if 0
enum shiver_soundfx
{
    SH_NULL_SOUND,
    SH_TEST_SOUND,
    SFX_COUNT
};

struct shiver_audio_engine
{
    ma_context sh_AudioContext;
    ma_context_config sh_AudioContextConfig;
    
    ma_device sh_AudioOutputDevice;
    ma_device_config sh_AudioDeviceConfig;
    
    ma_decoder sh_AudioDecoder;
    
    ma_resource_manager sh_AudioManager;
    ma_resource_manager_config sh_AudioManagerConfig;
    
    int32 CurrentSoundIndex;
    ma_resource_manager_data_source sh_AudioLoadedSounds[MAX_SOUNDS];
};
#endif

enum music_tracks
{
    SUNKEN_SEA,
    TRACK_COUNT
};

struct shiver_audio_engine
{
    ma_context AudioContext;
    ma_context_config AudioContextConfig;
    
    ma_device OutputDevice;
    ma_device_config OutputDeviceConfig;
    
    ma_resource_manager ResourceManager;
    ma_resource_manager_config ResourceManagerConfig;
    
    ma_engine Engine;
    ma_engine_config EngineConfig;
    
    uint32 CurrentTrackIdx;
    ma_sound BackgroundTracks[MAX_SOUNDS];
};

// NOTE(Sleepster): AUDIO ENGINE POINTER
global_variable shiver_audio_engine *AudioSubsystem;

internal inline void
sh_PlaySound(char *Filename)
{
    char Complete[50];
    char *Concat1 = "res/sounds/";
    char *Concat2 = ".wav";
    snprintf(Complete, sizeof(Complete), "%s%s%s\0", Concat1, Filename, Concat2);
    
    ma_engine_play_sound(&AudioSubsystem->Engine, Complete, 0);
}

internal inline void
sh_LoadBackgroundTrack(char *Filename)
{
    ma_result Result;
    char Complete[50] = {};
    char *Concat1 = "res/sounds/";
    char *Concat2 = ".mp3";
    snprintf(Complete, sizeof(Complete), "%s%s%s\0", Concat1, Filename, Concat2);
    
    Result = ma_sound_init_from_file(&AudioSubsystem->Engine, 
                                     Complete, 
                                     MA_SOUND_FLAG_DECODE| 
                                     MA_SOUND_FLAG_ASYNC | 
                                     MA_SOUND_FLAG_STREAM, 
                                     NULL, 
                                     NULL, 
                                     &AudioSubsystem->BackgroundTracks[AudioSubsystem->CurrentTrackIdx]);
    ++AudioSubsystem->CurrentTrackIdx;
}

internal inline void
sh_LoadBackgroundTracks()
{
    sh_LoadBackgroundTrack("Test");
}

internal void
sh_PlayBackgroundTrack(music_tracks Track)
{
    ma_result Result;
    
    if(!ma_sound_is_playing(&AudioSubsystem->BackgroundTracks[Track]))
    {
        ma_sound_set_looping(&AudioSubsystem->BackgroundTracks[Track], MA_TRUE);
        Result = ma_sound_start(&AudioSubsystem->BackgroundTracks[Track]);
        if(Result != MA_SUCCESS)
        {
            print_m("Failure to start the sound!, Code: %d\n", Result);
            Assert(false, "Failure\n");
        }
    }
}

#endif //_SHIVER__AUDIO_ENGINE_H
