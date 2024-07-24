/* date = June 17th 2024 8:58 am */

#ifndef _SHIVER_H
#define _SHIVER_H

#include "Shiver_Renderer.h"
#include "Intrinsics.h"
#include "Shiver_Input.h"
#include "Shiver_AudioEngine.h"
#include "Shiver_Globals.h"
#include "util/Math.h"

enum vertex_positions
{
    TOP_LEFT = 0,
    TOP_RIGHT = 1,
    BOTTOM_LEFT = 2,
    BOTTOM_RIGHT = 3
};

enum entity_flags
{
    IS_VALID  = 1 << 0,
    IS_SOLID  = 1 << 1,
    IS_ACTOR  = 1 << 2,
    IS_TILE   = 1 << 3,
    IS_ACTIVE = 1 << 4,
    ENTITY_FLAGS_COUNT
};

enum entity_archetype
{
    NIL = 0,
    PLAYER = 1,
    ROCK = 2,
    TREE00 = 3,
    TREE01 = 4,
    ARCH_COUNT
};

// TODO(Sleepster): Sprite Directions
struct entity
{
    entity_archetype Arch;
    uint32 Flags;
    
    static_sprite_data Sprite;
    
    vec2 Size;
    vec2 Position;
    vec2 Acceleration;
    
    real32 Speed;
    bool Collision;
    
    vec2 Vertex[MAX_COLLIDER_VERTS];
    int32 VertexCount;
};

struct simplex
{
    vec2 Vertex[MAX_SIMPLEX_VERTS];
    int32 VertexCount;
};

struct gjk_data
{
    bool32 Collision;
    simplex Simplex;
};

struct gjk_epa_data
{
    bool32 Collision;
    real64 Depth;
    vec2 CollisionNormal;
};

struct epa_edge
{
    vec2 Normal;
    int32 Index;
    real32 Distance;
};

struct time
{
    real32 DeltaTime;
    real32 CurrentTime;
    real32 NextTimestep;
    real32 Alpha;
    uint32 Seed;
};

struct game_memory
{
    MemoryArena TransientStorage;
    MemoryArena PermanentStorage;
};

struct world 
{
    entity Entities[MAX_ENTITIES]; 
};

struct gamestate
{
    KeyCodeID KeyCodeLookup[KEY_COUNT];
    Input GameInput;

    world World;
};

///////////////////////////////////////////////////
// NOTE(Sleepster): Game Sprite Getter
internal inline static_sprite_data
sh_glGetSprite(entity_archetype Arch, glrenderdata *RenderData)
{
    static_sprite_data Result = RenderData->StaticSprites[Arch];
    return(Result);
}

///////////////////////////////////////////////////
// NOTE(Sleepster): Sprite Loading and management
internal void
sh_glLoadSpriteSheet(glrenderdata *RenderData)
{
    RenderData->StaticSprites[NIL]    = {{0, 0}, {0, 0}};
    RenderData->StaticSprites[PLAYER] = {{0, 0}, {12, 11}};
    RenderData->StaticSprites[ROCK]   = {{16, 0}, {12, 8}};
    RenderData->StaticSprites[TREE00] = {{32, 0}, {7, 15}};
}

// NOTE(Sleepster): The rotation is in Degrees
internal void
sh_glDrawStaticSprite2D(entity_archetype Arch, vec2 Position, glrenderdata *RenderData)
{
    static_sprite_data SpriteData = sh_glGetSprite(Arch, RenderData);
    
    renderertransform Transform  = {};
    Transform.AtlasOffset = SpriteData.AtlasOffset;
    Transform.SpriteSize = SpriteData.SpriteSize;
    Transform.Size = v2Cast(RenderData->StaticSprites[Arch].SpriteSize);
    Transform.WorldPosition = Position - (Transform.Size / 2);
    
    RenderData->RendererTransforms[RenderData->TransformCounter++] = Transform;
}

// NOTE(Sleepster): Random Number Generation stuff
#define RAND_MAX_64 0xFFFFFFFFFFFFFFFFull
#define MULTIPLIER 6364136223846793005ull
#define INCREMENT 1442695040888963407ull 

internal inline uint64 
GetRandom()
{
    // NOTE(Sleepster): Look Ma, the only local_perist(s) in the entire program!
    local_persist uint64 rng_state = 1;
    uint64_t x = rng_state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    rng_state = x;
    return x;}

internal inline real32
GetRandomReal32()
{
    return((real32)GetRandom()/(real32)UINT64_MAX);
}

internal inline real32
GetRandomReal32_Range(real32 Minimum, real32 Maximum)
{
    return((Maximum - Minimum)*GetRandomReal32() + Minimum);
}

#define GAME_UPDATE_AND_RENDER(name) void name(gamestate *State, glrenderdata *RenderData, win32windowdata *WindowData, time Time, game_memory *Memory)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub)
{
}

#define GAME_FIXED_UPDATE(name) void name(gamestate *State, glrenderdata *RenderData, time Time, game_memory *Memory)
typedef GAME_FIXED_UPDATE(game_fixed_update);
GAME_FIXED_UPDATE(GameFixedUpdateStub)
{
}

#define GAME_ON_AWAKE(name) void name(gamestate *State, glrenderdata *RenderData, shiver_audio_engine *AudioEngineIn, game_memory *Memory)
typedef GAME_ON_AWAKE(game_on_awake);
GAME_ON_AWAKE(GameOnAwakeStub)
{
}

#endif //_SHIVER_H
