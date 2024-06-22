#include "Intrinsics.h"
#include "Shiver.h"
#include "Shiver_Input.h"
#include "Shiver_AudioEngine.h"

internal entity
CreateEntity(sprites SpriteID, vec2 Position, glrenderdata *RenderData)
{
    entity Entity = {};
    sh_glCreateStaticSprite2D({0, 0}, {16, 16}, SpriteID, RenderData);
    
    Entity.SpriteID = SpriteID;
    Entity.Sprite = sh_glGetSprite(SpriteID, RenderData);
    Entity.Position = Position;
    
    return(Entity);
}

internal inline void
DrawEntityStaticSprite2D(entity Entity, ivec2 Size, glrenderdata *RenderData)
{
    if(Entity.SpriteID != SPRITE_NULL)
    {
        sh_glDrawStaticSprite2D(Entity.SpriteID, Entity.Position, Size, RenderData);
    }
    return;
}

internal inline entity
DeleteEntity(void)
{
    entity Entity = {};
    return(Entity);
}

extern "C"
GAME_ON_AWAKE(GameOnAwake)
{
    State->Entities[0] = CreateEntity(SPRITE_DICE, {0, 0}, RenderData);
    sh_FMODPlaySoundFX(AudioSubsystem->SoundFX[SFX_TEST]);
}

extern "C"
GAME_FIXED_UPDATE(GameFixedUpdate)
{
}

extern "C"
GAME_UPDATE_AND_RENDER(GameUnlockedUpdate)
{
    DrawEntityStaticSprite2D(State->Entities[0], {16, 16}, RenderData);
}