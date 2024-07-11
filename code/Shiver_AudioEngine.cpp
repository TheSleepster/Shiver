/*
    This is not a final layer. (Some) Additional Requirements:

    COMPLETE
    - Basic loading from FMOD Studio
    - Integration with the FMOD Studio API
    - Play sounds through API calls on Events

    TODO
    - Audio tracks (Music)
    - A call for a looping sound that's terminated on condition

*/


#include "Intrinsics.h"
#include "Shiver_Globals.h"

#if FMOD
#include "../data/deps/FMOD/fmod.h"
#include "../data/deps/FMOD/fmod_common.h"

// FMOD STUDIO
#include "../data/deps/FMOD/fmod_studio.h"
#include "../data/deps/FMOD/fmod_studio_common.h"

internal void
sh_InitializeFMODStudioSubsystem(fmod_sound_subsystem_data *FMODSubsystemData)
{
    FMOD_RESULT Result;

    // NOTE(Sleepster): Core System
    Result = FMOD_Studio_System_Create(&FMODSubsystemData->StudioSystem, FMOD_VERSION);
    if(Result != FMOD_OK)
    {
        print_m("Error: %d\n", Result);
        Assert(false, "Failed to create the FMOD studio system!\n");
    }
    // NOTE(Sleepster): We only need to gather the core system if we want to assign changes to it
    Result = FMOD_Studio_System_GetCoreSystem(FMODSubsystemData->StudioSystem, &FMODSubsystemData->CoreSystem);
    if(Result != FMOD_OK)
    {
        print_m("Error: %d\n", Result);
        Assert(false, "Failed to get FMOD Studio's core system!\n");
    }
    Result = FMOD_System_SetSoftwareFormat(FMODSubsystemData->CoreSystem, 48000, FMOD_SPEAKERMODE_STEREO, 0);
    if(Result != FMOD_OK)
    {
        print_m("Error: %d\n", Result);
        Assert(false, "Failed to Set the FMOD System software format!\n");
    }

    // NOTE(Sleepster): Studio System
    Result = FMOD_Studio_System_Initialize(FMODSubsystemData->StudioSystem, 512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
    if(Result != FMOD_OK)
    {
        print_m("Error: %d\n", Result);
        Assert(false, "Failed to Intialize the FMOD studio system!\n");
    }
}

internal void
sh_LoadFMODStudioBankData(fmod_sound_subsystem_data *FMODSubsystemData,
                          const char *MainBankFilepath,
                          const char *StringsBankFilepath)
{
    FMOD_RESULT Result;

    FMODSubsystemData->MasterBankFilepath = MainBankFilepath;
    FMODSubsystemData->StringsBankFilepath = StringsBankFilepath;

    Result = FMOD_Studio_System_LoadBankFile(FMODSubsystemData->StudioSystem, FMODSubsystemData->MasterBankFilepath,
                                             FMOD_STUDIO_LOAD_BANK_NORMAL, &FMODSubsystemData->MasterBank);
    if(Result != FMOD_OK)
    {
        print_m("Error: %d\n", Result);
        Assert(false, "Failed to Load the main bank file!\n");
    }

    Result = FMOD_Studio_System_LoadBankFile(FMODSubsystemData->StudioSystem, FMODSubsystemData->StringsBankFilepath,
                                             FMOD_STUDIO_LOAD_BANK_NORMAL, &FMODSubsystemData->MasterStringsBank);
    if(Result != FMOD_OK)
    {
        print_m("Error: %d\n", Result);
        Assert(false, "Failed to Load the strings bank file!\n");
    }

    FMODSubsystemData->MasterBankLastWriteTime = Win32GetLastWriteTime(FMODSubsystemData->MasterBankFilepath);
    FMODSubsystemData->StringsBankLastWriteTime = Win32GetLastWriteTime(FMODSubsystemData->StringsBankFilepath);
}

// NOTE(Sleepster): This will load all of the sounds from the banks into the engine
internal inline void
sh_FMODStudioLoadSFXData(fmod_sound_subsystem_data *FMODSubsystemData,
                         fmod_sound_event *SoundEffects)
{
    FMOD_Studio_System_GetEvent(FMODSubsystemData->StudioSystem, "event:/Test", &SoundEffects[TEST_SFX].EventDesc);
    FMOD_Studio_System_GetEvent(FMODSubsystemData->StudioSystem, "event:/Boop", &SoundEffects[TEST_BOOP].EventDesc);
    FMOD_Studio_System_GetEvent(FMODSubsystemData->StudioSystem, "event:/Music", &SoundEffects[TEST_MUSIC].EventDesc);
}

internal void
sh_ReloadFMODStudioBankData(fmod_sound_subsystem_data *FMODSubsystemData,
                            fmod_sound_event *SoundEffects)
{
    // RELOAD THE MASTER BANK DATA
    FMOD_Studio_Bank_Unload(FMODSubsystemData->MasterBank);
    FMOD_Studio_System_LoadBankFile(FMODSubsystemData->StudioSystem, FMODSubsystemData->MasterBankFilepath,
                                    FMOD_STUDIO_LOAD_BANK_NORMAL, &FMODSubsystemData->MasterBank);
    // RELOAD THE STRINGS BANK DATA
    FMOD_Studio_Bank_Unload(FMODSubsystemData->MasterStringsBank);
    FMOD_Studio_System_LoadBankFile(FMODSubsystemData->StudioSystem, FMODSubsystemData->StringsBankFilepath,
                                    FMOD_STUDIO_LOAD_BANK_NORMAL, &FMODSubsystemData->MasterStringsBank);

    FMODSubsystemData->MasterBankLastWriteTime = Win32GetLastWriteTime(FMODSubsystemData->MasterBankFilepath);
    FMODSubsystemData->StringsBankLastWriteTime = Win32GetLastWriteTime(FMODSubsystemData->StringsBankFilepath);

    sh_FMODStudioLoadSFXData(FMODSubsystemData, SoundEffects);
}
#endif

// GAME HEADERS
#include "Win32_Shiver.h"
#include "Shiver_AudioEngine.h"

internal void
sh_AudioEngineCallback(ma_device *Device, void *Output, const void *Input, uint32 FrameCount)
{
    ma_decoder *Decoder = (ma_decoder*)Device->pUserData;
    ma_resource_manager *ResourceManager = (ma_resource_manager *)Device->pUserData;
    //Assert(!Decoder, "Decoder not Found");
    Assert(ResourceManager != 0, "ResourceManager not Found");

    ma_data_source_read_pcm_frames(ResourceManager, Output, FrameCount, 0);

    (void)Input;
}

internal bool32
sh_InitAudioEngine(shiver_audio_engine *sh_AudioEngine)
{
    // NOTE(Sleepster): Playback
    ma_device_info *PlaybackInfo;
    uint32 PlaybackCount;

    // NOTE(Sleepster): Capture
    ma_device_info *CaptureInfo;
    uint32 CaptureCount;
    ma_result Result;

    Result = ma_context_init(NULL, 0, NULL, &sh_AudioEngine->sh_AudioContext);
    if(Result != MA_SUCCESS)
    {
        print_m("Failed to Initialize the sh_AudioEngine's Context!, Code: %d\n", Result)
        Assert(false, "Failed to Initialize the sh_AudioEngine's Context!\n");
    }

    if(ma_context_get_devices(&sh_AudioEngine->sh_AudioContext,
                              &PlaybackInfo, &PlaybackCount,
                              &CaptureInfo, &CaptureCount) != MA_SUCCESS)
    {
        print_m("Failed to Gather the Input/Output Devices!, Code: %d\n", Result);
        Assert(false, "Failed to Gather the Input/Output Devices!\n");
    }

    for(uint32 DeviceIndex = 0;
        DeviceIndex < PlaybackCount;
        ++DeviceIndex)
    {
        print_m("%d - %s\n", DeviceIndex, PlaybackInfo[DeviceIndex].name);
    }

    sh_AudioEngine->sh_AudioDeviceConfig = ma_device_config_init(ma_device_type_playback);
    sh_AudioEngine->sh_AudioDeviceConfig.playback.format = ma_format_s32;
    sh_AudioEngine->sh_AudioDeviceConfig.playback.channels = CHANNEL_COUNT;
    sh_AudioEngine->sh_AudioDeviceConfig.sampleRate = SAMPLE_COUNT;
    sh_AudioEngine->sh_AudioDeviceConfig.dataCallback = sh_AudioEngineCallback;
    sh_AudioEngine->sh_AudioDeviceConfig.pUserData = &sh_AudioEngine->sh_AudioManagerSource;

    Result = ma_device_init(NULL,
                            &sh_AudioEngine->sh_AudioDeviceConfig,
                            &sh_AudioEngine->sh_AudioOutputDevice);
    if(Result != MA_SUCCESS)
    {
        print_m("Failure to initialize the device!, Code: %d\n", Result);
        Assert(false, "Failure\n");
    }

    sh_AudioEngine->sh_AudioManagerConfig = ma_resource_manager_config_init();
    sh_AudioEngine->sh_AudioManagerConfig.decodedFormat = sh_AudioEngine->sh_AudioOutputDevice.playback.format;
    sh_AudioEngine->sh_AudioManagerConfig.decodedChannels = sh_AudioEngine->sh_AudioOutputDevice.playback.channels;
    sh_AudioEngine->sh_AudioManagerConfig.decodedSampleRate = sh_AudioEngine->sh_AudioOutputDevice.sampleRate;

    Result = ma_resource_manager_init(&sh_AudioEngine->sh_AudioManagerConfig, &sh_AudioEngine->sh_AudioManager);
    if(Result != MA_SUCCESS)
    {
        print_m("Failure to initialize the Resource Manager!, Code: %d\n", Result);
        Assert(false, "Failure\n");
    }

    Result = ma_resource_manager_data_source_init(&sh_AudioEngine->sh_AudioManager,
                                                  "res/sounds/Test.mp3",
                                                  MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE|
                                                  MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC |
                                                  MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM,
                                                  0,
                                                  &sh_AudioEngine->sh_AudioManagerSource);
    if(Result != MA_SUCCESS)
    {
        print_m("Failure to initialize the Resource Manager!, Code: %d\n", Result);
        Assert(false, "Failure\n");
    }

    ma_data_source_set_looping(&sh_AudioEngine->sh_AudioManagerSource, MA_TRUE);
    ma_device_set_master_volume(&sh_AudioEngine->sh_AudioOutputDevice, 0.05f);

    Result = ma_device_start(&sh_AudioEngine->sh_AudioOutputDevice);
    if(Result != MA_SUCCESS)
    {
        print_m("Failure to start the device!, Code: %d\n", Result);
        Assert(false, "Failure\n");
    }

    return(1);
}
