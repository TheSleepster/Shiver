#include "Intrinsics.h"
#include "Shiver.h"
#include "Shiver_Input.h"
#include "Shiver_AudioEngine.h"

#define TILEMAP_SIZE_X 17
#define TILEMAP_SIZE_Y 9

#define TILESIZE 16

internal void 
UpdateEntityColliderData(entity *Entity)
{
    Entity->Vertex[TOP_LEFT]     = {Entity->Position};
    Entity->Vertex[TOP_RIGHT]    = {Entity->Position.x + Entity->Size.x, Entity->Position.y};
    Entity->Vertex[BOTTOM_LEFT]  = {Entity->Position.x, Entity->Position.y + Entity->Size.y};
    Entity->Vertex[BOTTOM_RIGHT] = {Entity->Position + Entity->Size};
}

// TODO(Sleepster): Make it so that we are not making a new sprite every frame, only when we need to actually make it
internal entity
CreateEntity(sprites SpriteID, vec2 Position, vec2 Size, glrenderdata *RenderData, gamestate *State)
{
    entity Entity = {};
    switch(SpriteID)
    {
        case SPRITE_DICE:
        {
            sh_glCreateStaticSprite2D({0, 0}, {16, 16}, SpriteID, RenderData);
        }break;
        case SPRITE_FLOOR:
        {
            sh_glCreateStaticSprite2D({16, 0}, {16, 16}, SpriteID, RenderData);
        }break;
        case SPRITE_WALL:
        {
            sh_glCreateStaticSprite2D({32, 0}, {16, 16}, SpriteID, RenderData);
        }break;
    }
    
    Entity.SpriteID = SpriteID;
    Entity.Sprite = sh_glGetSprite(SpriteID, RenderData);
    Entity.Position = Position;
    Entity.Size = Size;
    
    Entity.Vertex[TOP_LEFT]     = {Entity.Position};
    Entity.Vertex[TOP_RIGHT]    = {Entity.Position.x + Entity.Size.x, Entity.Position.y};
    Entity.Vertex[BOTTOM_LEFT]  = {Entity.Position.x, Entity.Position.y + Entity.Size.y};
    Entity.Vertex[BOTTOM_RIGHT] = {Entity.Position + Entity.Size};
    
    State->CurrentEntityCount++;
    return(Entity);
}

internal inline void
DrawEntityStaticSprite2D(entity Entity, glrenderdata *RenderData)
{
    if(Entity.SpriteID != SPRITE_NULL)
    {
        sh_glDrawStaticSprite2D(Entity.SpriteID, Entity.Position, iv2Cast(Entity.Size), RenderData);
    }
    return;
}

internal inline entity
DeleteEntity(void)
{
    entity Entity = {};
    return(Entity);
}

internal void
DrawTilemap(const uint8 Tilemap[TILEMAP_SIZE_Y][TILEMAP_SIZE_X], glrenderdata *RenderData, gamestate *State)
{
    sh_glCreateStaticSprite2D({16, 0}, {16, 16}, SPRITE_FLOOR, RenderData);
    sh_glCreateStaticSprite2D({32, 0}, {16, 16}, SPRITE_WALL, RenderData);
    for(int32 Row = 0;
        Row < TILEMAP_SIZE_Y;
        ++Row)
    {
        for(int32 Column = 0;
            Column < TILEMAP_SIZE_X;
            ++Column)
        {
            switch(Tilemap[Row][Column])
            {
                case 0:
                {
                }break;
                case 1:
                {
                    State->Entities[State->CurrentEntityCount] = 
                        CreateEntity(SPRITE_WALL, {real32(Column * TILESIZE), real32(Row * TILESIZE)}, {16, 16}, RenderData, State);
                }break;
            }
        }
    }
}

extern "C"
GAME_ON_AWAKE(GameOnAwake)
{
    const uint8 Tilemap[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = 
    {
        {1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1},
        {1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 1},
        {1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 1},
        {1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 1},
        {1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 1},
        {1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 1},
        {1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 1},
        {1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 1},
        {1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1}
    };
    
    State->Entities[0] = {};
    State->CurrentEntityCount = 0;
    State->Entities[1] = CreateEntity(SPRITE_DICE, {130, 60}, {16, 16}, RenderData, State);
    DrawTilemap(Tilemap, RenderData, State);
    //sh_FMODPlaySoundFX(AudioSubsystem->SoundFX[SFX_TEST]);
}

extern "C"
GAME_FIXED_UPDATE(GameFixedUpdate)
{
    const real32 MaxSpeed = 2;
    State->Entities[1].pPosition = State->Entities[1].Position;
    
    if(IsGameKeyDown(MOVE_UP, &State->GameInput))
    {
        State->Entities[1].Velocity.y += -MaxSpeed;
    }
    if(IsGameKeyDown(MOVE_DOWN, &State->GameInput))
    {
        State->Entities[1].Velocity.y += MaxSpeed;
    }
    if(IsGameKeyDown(MOVE_LEFT, &State->GameInput))
    {
        State->Entities[1].Velocity.x += -MaxSpeed;
    }
    if(IsGameKeyDown(MOVE_RIGHT, &State->GameInput))
    {
        State->Entities[1].Velocity.x += MaxSpeed;
    }
    State->Entities[1].Position.x += State->Entities[1].Velocity.x;
    State->Entities[1].Position.y += State->Entities[1].Velocity.y;
    State->Entities[1].Position = v2Lerp(State->Entities[1].pPosition, State->Entities[1].Position, DeltaTime);
    
    State->Entities[1].Velocity = {};
}

extern "C"
GAME_UPDATE_AND_RENDER(GameUnlockedUpdate)
{
    RenderData->Cameras[CAMERA_GAME].Position = {130, -60};
    DrawEntityStaticSprite2D(State->Entities[1], RenderData);
    
    for(int32 Index = 2;
        Index <= State->CurrentEntityCount;
        ++Index)
    {
        DrawEntityStaticSprite2D(State->Entities[Index], RenderData);
    }
}