#include "Intrinsics.h"
#include "Shiver.h"

#define TILEMAP_SIZE_X 17
#define TILEMAP_SIZE_Y 9

#define TILESIZE 16
#define EPSILON 0.00000000000005

// NOTE(Sleepster): The Simplex is no longer just 3 points since we are now using EPA for the distance and normal calculations
struct simplex
{
    vec2 Points[32];
    int32 VertexCount;
};

struct gjk_data
{
    bool32 Collided;
    simplex Simplex;
};

struct epa_data
{
    bool32 Collided;
    vec2 CollisionNormal;
    real32 Distance;
};

struct epa_edge
{
    vec2 Normal;
    int32 Index;
    real32 Distance;
};

internal vec2
AveragePoint(entity *Entity) 
{
    vec2 Average = {};
    
    for(int Index = 0; 
        Index < Entity->VertexCount; 
        ++Index) 
    {
        Average.x += Entity->Vertex[Index].x;
        Average.y += Entity->Vertex[Index].y;
    }
    Average.x /= Entity->VertexCount;
    Average.y /= Entity->VertexCount;
    
    return(Average);
}

internal bool32
FurthestPoint(entity *Entity, vec2 Direction) 
{ 
    int BestIndex = 0;
    real32 MaxProduct = v2Dot(Entity->Vertex[0], Direction);
    vec2 Result = {};
    for(int Point = 1; 
        Point < Entity->VertexCount; 
        ++Point) 
    { 
        real32 Product = v2Dot(Entity->Vertex[Point], Direction); 
        if(Product > MaxProduct) 
        {
            MaxProduct = Product;
            BestIndex = Point;
        }
    }
    return(BestIndex);
}

internal vec2
GJKSupport(entity *A, entity *B, vec2 Direction) 
{
    int AMax = FurthestPoint(A, Direction);
    int BMax = FurthestPoint(B, -Direction);
    
    return(A->Vertex[AMax] - B->Vertex[BMax]);
}

internal bool32
GJK(entity *A, entity *B) 
{
    vec2 Simplex[32] = {};
    vec2 Position1 = AveragePoint(A);
    vec2 Position2 = AveragePoint(B);
    vec2 Direction = Position1 - Position2; // Set the direction to the point in the Minkowski Difference
    
    vec2 RegionA;
    vec2 RegionB;
    vec2 RegionC;
    vec2 AO;
    vec2 AB;
    vec2 AC;
    vec2 ACPerp;
    vec2 ABPerp;
    
    // If Zero set to any arbitray Axis
    if((Direction.x == 0) && (Direction.y == 0)) 
    {
        Direction.x = 1.0f;
    }
    
    // Set the First Support as the new point of the Simplex
    Simplex[0] = GJKSupport(A, B, Direction);
    if(v2Dot(Simplex[0], Direction) <= 0) 
    {
        return(0);
        // Collision not possible
    }
    
    // Invert the vector so the next search direction is flipped towards the origin
    Direction = v2Inverse(Direction);
    
    int Index = 0;
    for(;;) 
    {
        RegionA = Simplex[++Index] = GJKSupport(A, B, Direction);
        if(v2Dot(RegionA, Direction) <= 0) 
        {
            return(0);
            // Collision not possible
        }
        
        AO = v2Inverse(RegionA); // From point A to Origin is likely just negative A
        // Simplex has 2 points, it is not a triangle yet.
        if(Index < 2) 
        {
            RegionB = Simplex[0];
            AB = RegionB - RegionA;
            Direction = v2TripleProduct(AB, AO, AB);
            if(v2LengthSq(Direction) == 0) 
            {
                Direction = v2Perpendicular(AB);
            }
            continue;
        } 
        
        RegionB = Simplex[1];
        RegionC = Simplex[0];
        
        AB = RegionB - RegionA; // Normal from AB to the origin
        AC = RegionC - RegionA;
        
        ACPerp = v2TripleProduct(AB, AC, AC);
        if(v2Dot(ACPerp, AO) >= 0) 
        { 
            Direction = ACPerp;
        }
        else 
        {
            ABPerp = v2TripleProduct(AC, AB, AB);
            if(v2Dot(ABPerp, AO) < 0) 
            {
                return(1);
                //Collision
            }
            
            Simplex[0] = Simplex[1]; // Swap the First Element (C);
            Direction = ABPerp; // New Direction is normal to AB -> Origin
        }
        
        Simplex[1] = Simplex[2]; // Swap Elements
        --Index;
    }
}

internal void 
UpdateEntityColliderData(entity *Entity)
{
    Entity->Vertex[TOP_LEFT]     = {Entity->Position};
    Entity->Vertex[TOP_RIGHT]    = {Entity->Position.x + Entity->Size.x, Entity->Position.y};
    Entity->Vertex[BOTTOM_LEFT]  = {Entity->Position.x, Entity->Position.y + Entity->Size.y};
    Entity->Vertex[BOTTOM_RIGHT] = {Entity->Position + Entity->Size};
}
// NOTE(Sleepster): END OF GJK


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
    Entity.VertexCount = 4;
    
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
        {1,0,0,0, 1,1,1,1, 0,1,1,0, 0,0,0,0, 1},
        {1,0,0,0, 1,0,0,0, 0,0,1,0, 0,0,0,0, 1},
        {1,0,0,0, 1,0,0,0, 0,0,1,0, 0,0,0,0, 1},
        {1,0,0,0, 1,0,0,0, 0,0,1,0, 0,0,0,0, 1},
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
        UpdateEntityColliderData(&State->Entities[1]);
    }
    
    for(int32 EntityIndex = 2;
        EntityIndex < State->CurrentEntityCount;
        ++EntityIndex)
    {
        bool Collided = GJK(&State->Entities[1], &State->Entities[EntityIndex]);
        real32 EntityDistance = v2Distance(State->Entities[1].Position, State->Entities[EntityIndex].Position);
        
        if(Collided != 0)
        {
            real32 TOI = {};
            Trace("Collision!\n");
        }
    }
}