#include "Intrinsics.h"
#include "Shiver.h"

#define TILEMAP_SIZE_X 17
#define TILEMAP_SIZE_Y 9

#define TILESIZE 16
#define EPSILON 0.00000000000005

// NOTE(Sleepster): The Simplex is no longer just 3 points since we are now using EPA for the distance and normal calculations
struct simplex
{
    vec2 Vertex[32];
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
    real32 Distance;
    vec2 CollisionNormal;
};

struct epa_edge
{
    vec2 Normal;
    int32 Index;
    real32 Distance;
};

// NOTE(Sleepster): This function updates both the simplex and the direction vector
internal bool
GJK_ComputeSimplexData(simplex *Simplex, vec2 *Direction)
{
    bool Result = 0;
    switch(Simplex->VertexCount)
    {
        // NOTE(Sleepster): One Vertex, Invalid
        case 1:
        {
            Assert(false, "Invalid code path, Simplex has one Point");
        }break;
        
        // NOTE(Sleepster): Two Vertices, find a third to create the triangle Simplex
        case 2:
        {
            vec2 A = Simplex->Vertex[1];
            vec2 B = Simplex->Vertex[0];
            vec2 AO = v2Inverse(A);
            vec2 AB = A - B;
            if(v2Dot(AB, AO)) // Is Within bounds of Minkowski sum
            {
                vec2 NewDirection = v2Perp(AB);
                if(v2Dot(NewDirection, AO)) // Wrong direction
                {
                    NewDirection = v2Invert(AB);
                    // NOTE(Sleepster): Wind clockwise
                    Simplex->Vertex[0] = A;
                    Simplex->Vertex[1] = B;
                }
                *Direction = NewDirection;
            }
            else
            {
                // NOTE(Sleepster): Readjust the direction and try again.
                Simplex->Vertex[0] = A;
                Simplex->VertexCount = 1;
                *Direction = AO;
            }
        }break;
        
        // NOTE(Sleepster): Triangle Simplex Created, Test
        case 3:
        {
            vec2 A = Simplex->Vertex[2];
            vec2 B = Simplex->Vertex[1];
            vec2 C = Simplex->Vertex[0];
            vec2 AO = v2Inverse(A);
            vec2 AB = A - B;
            vec2 AC = C - A;
            // NOTE(Sleepster): Check for Clockwise Winding
            if(v2Dot(v2Perp(AB), AC) > 0)
            {
                B = Simplex->Vertex[0];
                C = Simplex->Vertex[1];
                
                vec2 Temp = AB;
                AB = AC;
                AC = Temp;
            }
            // NOTE(Sleepster): Checks
            vec2 ACPerp = v2Perp(AC);
            vec2 ABPerp = v2Inverse(v2Perp(AB));
            
            bool OnLeft = (v2Dot(ACPerp, AO) > 0);
            bool OnRight = (v2Dot(ABPerp, AO) > 0);
            bool SimpleCase = 0;
            
            // NOTE(Sleepster): Determine which direction to check relative to the origin
            if(!OnRight && !OnLeft)
            {
                Result = 1;
                break;
            }
            else if(OnLeft)
            {
                // NOTE(Sleepster): If it is on the left, than we have gone to far from the origin. Readjust and try again.
                if(v2Dot(AC, AO) > 0)
                {
                    *Direction = ACPerp;
                    // NOTE(Sleepster): Swap the first and third vertices and try again
                    Simplex->Vertex[0] = C;
                    Simplex->Vertex[1] = A;
                    Simplex->VertexCount = 2;
                }
                else
                {
                    SimpleCase = 1;
                }
            }
            else if(OnRight)
            {
                if(v2Dot(AB, AO) > 0)
                {
                    *Direction = ABPerp;
                    // NOTE(Sleepster): Swap the first and second vertices and try again.
                    Simplex->Vertex[1] = B;
                    Simplex->Vertex[0] = A;
                    Simplex->VertexCount = 2;
                }
                else
                {
                    SimpleCase = 1;
                }
            }
            if(SimpleCase)
            {
                // NOTE(Sleepster): Failure. Restart completely.
                Simplex->Vertex[0] = A;
                Simplex->VertexCount = 1;
                *Direction = AO;
            }
        }break;
        default:
        {
            // NOTE(Sleepster): Something went SERIOUSLY Wrong
            Assert(false, "How?\n");
        }break;
    }
    return(Result);
}

internal bool32
GJK_FurthestPoint(entity *Entity, vec2 Direction) 
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
GJK_Support(entity *A, entity *B, vec2 Direction) 
{
    // NOTE(Sleepster): Find the most extreme point on each entity
    int AMax = GJK_FurthestPoint(A, Direction);
    int BMax = GJK_FurthestPoint(B, -Direction);
    
    return(A->Vertex[AMax] - B->Vertex[BMax]);
}

internal gjk_data
HandleGJK(entity *A, entity *B)
{
    bool Result = {};
    gjk_data GJKData = {};
    // NOTE(Sleepster): Early return if there is no full shape.
    if(A->VertexCount < 3 || B->VertexCount < 3) return(GJKData);
    
    // NOTE(Sleepster): Initial Point to start with. Initial direction does not matter
    vec2 iSupportPoint = GJK_Support(A, B, {1, 0}); 
    
    simplex Simplex = {};
    Simplex.Vertex[0] = iSupportPoint;
    Simplex.VertexCount = 1;
    // NOTE(Sleepster): Invert Initial direction to begin the search
    vec2 Direction = v2Invert(iSupportPoint);
    for(;;)
    {
        vec2 Point = GJK_Support(A, B, Direction);
        // NOTE(Sleepster): Early exit, Collision cannot occur
        if(v2Dot(Point, Direction) < 0)
        {
            Result = 0;
            break;
        }
        Simplex.Vertex[Simplex.VertexCount++] = Point;
        dAssert(Simplex.VertexCount >= 2 && Simplex.VertexCount <= 3); // control our vertices here
        if(GJK_ComputeSimplexData(&Simplex, &Direction))
        {
            Result = 1;
            break;
        }
    }
    
    GJKData.Collision = Result;
    GJKData.Simplex = Simplex;
    return(GJKData);
}

internal gjk_epa_data
GJK_EPA(entity *A, entity *B)
{
    gjk_epa_data CollisionInfo = {};
    gjk_data GJKInfo = HandleGJK(A, B);
}

internal bool
GJK(entity *A, entity *B)
{
    gjk_data Result = HandleGJK(A, B);
    return(Result.Collision);
}

// NOTE(Sleepster): END OF GJK EPA

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
        bool Collision = GJK(&State->Entities[1], &State->Entities[EntityIndex]);
        if(Collision)
        {
            Trace("Collision!\n");
        }
    }
}