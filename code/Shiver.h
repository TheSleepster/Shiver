/* date = June 17th 2024 8:58 am */

#ifndef _SHIVER_H
#define _SHIVER_H

#include "Intrinsics.h"
#include "Shiver_Input.h"
#include "Shiver_Renderer.h"
#include "Shiver_AudioEngine.h"

#define WORLD_WIDTH 160
#define WORLD_HEIGHT 90

enum VertexPositions
{
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

enum entityflags
{
    Is_Static = 0x01,
    Is_Player = 0x02,
    FlagCount,
};

// TODO(Sleepster): Sprite Directions
struct entity
{
    uint64 Flags;
    
    sprites SpriteID;
    spritedata Sprite;
    
    vec2 pPosition;
    vec2 Position;
    
    vec2 pVelocity;
    vec2 Velocity;
    vec2 Size;
    
    int32 VertexCount;
    vec2 Vertex[4];
    
    // NOTE(Sleepster): This is spooky
    real32 Rotation;
    
    real32 Mass;
    real32 InvMass;
    real32 Restitution;
};

struct gamestate
{
    KeyCodeID KeyCodeLookup[KEY_COUNT];
    Input GameInput;
    
    int32 CurrentEntityCount;
    entity Entities[256];
};

#define GAME_UPDATE_AND_RENDER(name) void name(gamestate *State, glrenderdata *RenderData, fmod_sound_subsystem_data *AudioSubsystem,  real32 DeltaTime)
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