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

#include "../data/deps/FMOD/fmod.h"
#include "../data/deps/FMOD/fmod_common.h"

// FMOD STUDIO
#include "../data/deps/FMOD/fmod_studio.h"
#include "../data/deps/FMOD/fmod_studio_common.h"

// GAME HEADERS
#include "Win32_Shiver.h"
#include "Shiver_AudioEngine.h"

internal void
sh_InitializeFMODStudioSubsystem(fmod_sound_subsystem_data *FMODSubsystemData)
{
    FMOD_RESULT Result;
    
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
