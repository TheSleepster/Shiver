/* date = June 17th 2024 8:58 am */

#ifndef _SHIVER_H
#define _SHIVER_H

#include "Intrinsics.h"
#include "Shiver_Input.h"
#include "Shiver_Renderer.h"
#include "Shiver_AudioEngine.h"
#include "Shiver_Globals.h"

enum VertexPositions
{
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

enum entityflags
{
    IS_SOLID = 0x01,
    IS_ACTOR = 0x02,
    IS_ACTIVE = 0x03,
    FlagCount
};

// TODO(Sleepster): Sprite Directions
struct entity
{
    uint32 Flags;
    
    static_sprites SpriteID;
    static_sprite_data Sprite;
    
    vec2 Size;
    vec2 Position;
    vec2 Velocity;
    vec2 Acceleration;
    vec2 AppliedForce;
    vec2 Vertex[4];
    
    int32 VertexCount;
    
    real32 Mass;
    real32 InvMass;
    
    // NOTE(Sleepster): This is spooky
    real32 Radius;
    real32 Rotation;
    real32 Inertia;
    real32 InvInertia;
    
    real32 StaticFriction;
    real32 DynamicFriction;
    
    real32 Density;
    real32 Restitution;
};

struct time
{
    real32 DeltaTime;
    real32 CurrentTime;
    real32 NextTimestep;
    real32 Alpha;
};

struct game_memory
{
    MemoryArena TransientStorage;
    MemoryArena PermanentStorage;
};

struct gamestate
{
    KeyCodeID KeyCodeLookup[KEY_COUNT];
    Input GameInput;
    
    int32 CurrentEntityCount;
    entity Entities[256];
};

///////////////////////////////////////////////////
// NOTE(Sleepster): Game Sprite Getter
internal inline static_sprite_data
sh_glGetSprite(static_sprites SpriteID, glrenderdata *RenderData)
{
    static_sprite_data Result = RenderData->StaticSprites[SpriteID];
    return(Result);
}

///////////////////////////////////////////////////
// NOTE(Sleepster): Sprite Loading and management
internal void
sh_glLoadSpriteSheet(glrenderdata *RenderData)
{
    RenderData->StaticSprites[SPRITE_NULL] = {{0, 0}, {0, 0}};
    RenderData->StaticSprites[SPRITE_DICE] = {{0, 0}, {16, 16}};
    RenderData->StaticSprites[SPRITE_FLOOR] = {{16, 0}, {16, 16}};
    RenderData->StaticSprites[SPRITE_WALL] = {{32, 0}, {16, 16}};
}

internal void
sh_glDrawStaticSprite2D(static_sprites SpriteID, vec2 Position, ivec2 Size, glrenderdata *RenderData)
{
    static_sprite_data SpriteData = sh_glGetSprite(SpriteID, RenderData);
    
    renderertransform Transform  = {};
    Transform.AtlasOffset = SpriteData.AtlasOffset;
    Transform.SpriteSize = SpriteData.SpriteSize;
    Transform.Size = v2Cast(Size);
    Transform.WorldPosition = Position - (Transform.Size / 2);
    RenderData->RendererTransforms[RenderData->TransformCounter++] = Transform;
}

#define GAME_UPDATE_AND_RENDER(name) void name(gamestate *State, glrenderdata *RenderData, fmod_sound_subsystem_data *AudioSubsystem, time Time, game_memory *Memory)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub)
{
}

#define GAME_FIXED_UPDATE(name) void name(gamestate *State, glrenderdata *RenderData, time Time, game_memory *Memory)
typedef GAME_FIXED_UPDATE(game_fixed_update);
GAME_FIXED_UPDATE(GameFixedUpdateStub)
{
}

#define GAME_ON_AWAKE(name) void name(gamestate *State, glrenderdata *RenderData, fmod_sound_subsystem_data *AudioSubsystem, game_memory *Memory)
typedef GAME_ON_AWAKE(game_on_awake);
GAME_ON_AWAKE(GameOnAwakeStub)
{
}

#endif //_SHIVER_H