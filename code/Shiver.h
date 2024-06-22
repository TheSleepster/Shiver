/* date = June 17th 2024 8:58 am */

#ifndef _SHIVER_H
#define _SHIVER_H

#include "Intrinsics.h"
#include "Shiver_Input.h"
#include "Shiver_Renderer.h"
#include "Shiver_AudioEngine.h"

#define WORLD_WIDTH 160
#define WORLD_HEIGHT 90

struct entity
{
    sprites SpriteID;
    spritedata Sprite;
    
    vec2 Position;
    vec2 Velocity;
};

struct gamestate
{
    KeyCodeID KeyCodeLookup[KEY_COUNT];
    Input GameInput;
    
    int32 CurrentEntityCount;
    entity Entities[256];
};

#define GAME_UPDATE_AND_RENDER(name) void name(gamestate *State, glrenderdata *RenderData, fmod_sound_subsystem_data *AudioSubsystem)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub)
{
}

#define GAME_FIXED_UPDATE(name) void name(gamestate *State, glrenderdata *RenderData, real32 DeltaTime)
typedef GAME_FIXED_UPDATE(game_fixed_update);
GAME_FIXED_UPDATE(GameFixedUpdateStub)
{
}

#define GAME_ON_AWAKE(name) void name(gamestate *State, glrenderdata *RenderData, fmod_sound_subsystem_data *AudioSubsystem)
typedef GAME_ON_AWAKE(game_on_awake);
GAME_ON_AWAKE(GameOnAwakeStub)
{
}

#endif //_SHIVER_H