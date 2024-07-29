#include "Intrinsics.h"
#include "Shiver.h"
#include "Shiver_AudioEngine.h"
#include "Shiver_Globals.h"
#include "util/Math.h"
#include "util/ShiverArray.h"

// Solving order
// - Do Caveman
// - Wait until performance :(
// - Fix with less Caveman
// - Unga bunga

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
    return x;
}

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

// NOTE(Sleepster): Perhaps since Velocity no longer exists, we could instead change it by the inverse of the difference of the New Player Position
// and the old player position
internal void
ResolveSolidCollision(entity *A, gjk_epa_data Data)
{
    vec2 PenVector = Data.CollisionNormal * (real32)Data.Depth;
    
    if(Data.CollisionNormal.y == 1)
    {
        A->Position.y += -real32(EPSILON + Data.Depth);
    }
    else if(Data.CollisionNormal.y == -1)
    {
        A->Position.y += real32(EPSILON + Data.Depth);
    }
    
    if(Data.CollisionNormal.x == 1)
    {
        A->Position.x += -real32(EPSILON + Data.Depth);
    }
    else if(Data.CollisionNormal.x == -1)
    {
        A->Position.x += real32(EPSILON + Data.Depth);
    }
    A->Acceleration = {0.0f, 0.0f};
}

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
            vec2 AO = v2Invert(A);
            vec2 AB = B - A;
            if(v2Dot(AB, AO)) // Is Within bounds of Minkowski sum
            {
                vec2 NewDirection = v2Perp(AB);
                if(v2Dot(NewDirection, AO) < 0) // Wrong direction
                {
                    NewDirection = v2Invert(NewDirection);
                    Check(v2Dot(NewDirection, AO) >= 0, "Invalid code path\n");
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
        
        // NOTE(Sleepster): The Triangle Simplex Created, Tested below.
        case 3:
        {
            vec2 A = Simplex->Vertex[2];
            vec2 B = Simplex->Vertex[1];
            vec2 C = Simplex->Vertex[0];
            vec2 AO = v2Invert(A);
            vec2 AB = B - A;
            vec2 AC = C - A;
            // NOTE(Sleepster): Check for Clockwise Winding
            if(v2Dot(v2Perp(AC), AB) > 0)
            {
                Trace("Invalid code path, if(v2Dot(v2Perp(AC), AB) > 0) on LN 118\n");
                B = Simplex->Vertex[0];
                C = Simplex->Vertex[1];
                
                vec2 Temp = AB;
                AB = AC;
                AC = Temp;
                Assert(v2Dot(v2Perp(AB), AC) > 0, "What?\n");
            }
            // NOTE(Sleepster): Checks
            vec2 ACPerp = v2Perp(AC);
            vec2 ABPerp = v2Invert(v2Perp(AB));
            
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
                Assert(!OnRight, "On right as well?\n");
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
                Assert(!OnLeft, "On left as well?\n");
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
    // NOTE(Sleepster): Early return if there is a full shape.
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

internal void
EPA_AddToSimplex(simplex *Simplex, vec2 Point, int32 InsertionIndex)
{
    if(Simplex->VertexCount >= ArrayCount(Simplex->Vertex))
    {
        Assert(false, "Simplex has grown too large to handle\n");
    }
    for(int32 VertexIndex = Simplex->VertexCount - 1;
        VertexIndex >= InsertionIndex;
        --VertexIndex)
    {
        Simplex->Vertex[VertexIndex + 1] = Simplex->Vertex[VertexIndex];
    }
    
    ++Simplex->VertexCount;
    Simplex->Vertex[InsertionIndex] = Point;
}

// NOTE(Sleepster): Expanded Polytope Algorithm
internal gjk_epa_data
GJK_EPA(entity *A, entity *B)
{
    gjk_epa_data CollisionInfo = {};
    gjk_data GJKInfo = HandleGJK(A, B);
    CollisionInfo.Collision = GJKInfo.Collision;
    if(GJKInfo.Collision)
    {
        simplex *Simplex = &GJKInfo.Simplex;
        for(;;)
        {
            // NOTE(Sleepster): Get the edge closest to the origin of our Minkowski difference,
            //                  Assume Clockwise
            epa_edge Edge = {};
            for(int32 VertexIndex = 0;
                VertexIndex < Simplex->VertexCount;
                ++VertexIndex)
            {
                int32 IndexA = VertexIndex;
                int32 IndexB = (VertexIndex == (Simplex->VertexCount - 1)) ? 0 : VertexIndex + 1; // Wrap the index;
                
                vec2 PointA = Simplex->Vertex[IndexA];
                vec2 PointB = Simplex->Vertex[IndexB];
                vec2 AO = v2Invert(PointA);
                vec2 AB = {PointB.x - PointA.x, PointB.y - PointA.y};
                
                // NOTE(Sleepster): This the edge normal that points AWAY from the origin when the simplex is clockwise oriented
                vec2 EdgeNormal = v2Normalize(v2Perp(AB));
                if(v2Dot(EdgeNormal, AO) > 0)
                {
                    EdgeNormal = v2Invert(EdgeNormal);
                }
                
                // NOTE(Sleepster): Find the Distance from the edge. The closest distance is always the perpindicular distance from the edge.
                real32 Distance = v2Dot(EdgeNormal, AO);
                
                Assert(Distance <= 0, "Distance is not less than zero\n");
                Distance *= -1; // Invert Distance for new search
                Assert(Distance >= 0, "Distance is not greater than zero\n");
                
                if(Distance < Edge.Distance || VertexIndex == 0)
                {
                    // NOTE(Sleepster): Shorter edge found
                    Edge.Distance = Distance;
                    Edge.Index = IndexB;
                    Edge.Normal = EdgeNormal;
                }
            }
            vec2 EPAPoint = GJK_Support(A, B, Edge.Normal);
            real64 FloatDistance = v2Dot(EPAPoint, Edge.Normal);
            
            // NOTE(Sleepster): See if these points are the same as previously acquired ones. If they are, we have our solution
            if(FloatDistance - Edge.Distance < GJK_TOLERANCE)
            {
                // NOTE(Sleepster): Invert the edge normal so that it is pointing towards our Minkowski Difference's origin
                CollisionInfo.CollisionNormal = Edge.Normal;
                CollisionInfo.Depth = FloatDistance;
                break;
            }
            else
            {
                // NOTE(Sleepster): If we're here, we haven't reached the edge of our Minkowski Difference.
                //                  Continue by adding points to the simplex in between the two points that made the closest edge.
                EPA_AddToSimplex(&GJKInfo.Simplex, EPAPoint, Edge.Index + 1);
            }
        }
    }
    return(CollisionInfo);
}

internal bool
GJK(entity *A, entity *B)
{
    gjk_data Result = HandleGJK(A, B);
    return(Result.Collision);
}

internal void
UpdatePlayerPosition(entity *Player, gamestate *State, time Time)
{
    Player->Acceleration = {};
    if(IsGameKeyDown(MOVE_UP, &State->GameInput))
    {
        Player->Acceleration.y = -1.0f;
    }
    
    else if(IsGameKeyDown(MOVE_DOWN, &State->GameInput))
    {
        Player->Acceleration.y = 1.0f;
    }
    
    if(IsGameKeyDown(MOVE_LEFT, &State->GameInput))
    {
        Player->Acceleration.x = -1.0f;
    }
    
    else if(IsGameKeyDown(MOVE_RIGHT, &State->GameInput))
    {
        Player->Acceleration.x = 1.0f;
    }

    v2Normalize(Player->Acceleration);
    vec2 OldPlayerP = Player->Position;
    real32 Friction = 0.9f;

    vec2 NextPos = { Player->Position.x + (Player->Position.x - OldPlayerP.x) + (Player->Speed * Player->Acceleration.x) * Square(Time.DeltaTime),
                     Player->Position.y + (Player->Position.y - OldPlayerP.y) + (Player->Speed * Player->Acceleration.y) * Square(Time.DeltaTime)};
    Player->Position = v2Lerp(NextPos, OldPlayerP, Time.DeltaTime);
}

internal void
DrawTilemap(const uint8 Tilemap[TILEMAP_SIZE_Y][TILEMAP_SIZE_X], glrenderdata *RenderData, gamestate *State)
{
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

internal inline void
DrawEntityStaticSprite2D(entity *Entity, vec4 Color, int RenderingOptions, glrenderdata *RenderData)
{
    sh_glDrawStaticSprite2D(Entity->Arch, Entity->Position, Color, RenderingOptions, RenderData);
}

internal void 
SetupPlayer(entity *Entity, glrenderdata *RenderData)
{
    Entity->Arch = PLAYER;
    Entity->Flags = IS_ACTIVE|IS_VALID|IS_ACTOR;

    Entity->Position = vec2{0, 0};
    Entity->Speed = 10.0f;
            
    Entity->Vertex[TOP_LEFT]     = {Entity->Position};
    Entity->Vertex[TOP_RIGHT]    = {Entity->Position.x + Entity->Size.x, Entity->Position.y};
    Entity->Vertex[BOTTOM_LEFT]  = {Entity->Position.x, Entity->Position.y + Entity->Size.y};
    Entity->Vertex[BOTTOM_RIGHT] = {Entity->Position + Entity->Size};
    Entity->VertexCount = 4;
}

internal void 
SetupRock(entity *Entity, glrenderdata *RenderData)
{
    Entity->Arch = ROCK;
    Entity->Flags = IS_ACTIVE|IS_VALID|IS_ACTOR;

    Entity->Position = vec2{0, 0};
    Entity->Speed = 2.0f;
            
    Entity->Vertex[TOP_LEFT]     = {Entity->Position};
    Entity->Vertex[TOP_RIGHT]    = {Entity->Position.x + Entity->Size.x, Entity->Position.y};
    Entity->Vertex[BOTTOM_LEFT]  = {Entity->Position.x, Entity->Position.y + Entity->Size.y};
    Entity->Vertex[BOTTOM_RIGHT] = {Entity->Position + Entity->Size};
    Entity->VertexCount = 4;
}

internal void 
SetupTree(entity *Entity, glrenderdata *RenderData)
{
    Entity->Arch = TREE00;
    Entity->Flags = IS_ACTIVE|IS_VALID|IS_ACTOR;

    Entity->Position = vec2{0, 0};
    Entity->Speed = 1.0f;
            
    Entity->Vertex[TOP_LEFT]     = {Entity->Position};
    Entity->Vertex[TOP_RIGHT]    = {Entity->Position.x + Entity->Size.x, Entity->Position.y};
    Entity->Vertex[BOTTOM_LEFT]  = {Entity->Position.x, Entity->Position.y + Entity->Size.y};
    Entity->Vertex[BOTTOM_RIGHT] = {Entity->Position + Entity->Size};
    Entity->VertexCount = 4;
}

internal entity*
CreateEntity(world *World)
{
    entity *Result = {};
    for(uint32 EntityIndex = 0;
        EntityIndex < MAX_ENTITIES;
        ++EntityIndex)
    {
        entity *Found = &World->Entities[EntityIndex];
        if(!(Found->Flags & IS_VALID))
        {
            Result = Found; 
            break;
        }
    }
    Assert(Result, "No more entities\n");

    ++World->EntityCounter;
    Result->Flags = IS_VALID;

    return(Result);
}

internal inline void
DeleteEntity(entity *Entity)
{
    memset(Entity, 0, sizeof(struct entity));
}

// TODO(Sleepster): Make this actually work some day, I don't understand matrices
internal vec2
MouseToWorldSpace(Input *GameInput, win32windowdata *WindowData, glrenderdata *RenderData)
{
    KeyboardInput *MouseData = &GameInput->Keyboard;

    if(WindowData->SizeData.Width != 0)
    {
        // NOTE(Sleepster): get the NDC (Normalized Device Coordinates) 
        vec2 ndc = {(MouseData->CurrentMouse.x / (WindowData->SizeData.Width * 0.5f)) - 1.0f, 
                   (MouseData->CurrentMouse.y / (WindowData->SizeData.Height * 0.5f)) - 1.0f};

        vec4 WorldPos = vec4{ndc.x, ndc.y, 0.0f, 1.0f};
        WorldPos = mat4Transform(mat4Inverse(RenderData->GameCamera.Matrix), WorldPos);
        WorldPos = mat4Transform(mat4Inverse(RenderData->ViewMatrix), WorldPos);

        return(vec2{WorldPos.x, WorldPos.y});
    }
    return(vec2{0.0f, 0.0f});
}

global_variable entity *Player = {};

external
GAME_ON_AWAKE(GameOnAwake)
{
    AudioSubsystem = AudioEngineIn;
    sh_glLoadSpriteSheet(RenderData);
    
    for(uint32 EntityIndex = 0;
        EntityIndex < MAX_ENTITIES;
        ++EntityIndex)
    {
        entity *temp = &State->World.Entities[EntityIndex];
        DeleteEntity(temp);
    }

    Player = CreateEntity(&State->World);
    SetupPlayer(Player, RenderData);

    for(uint32 Index = 0;
        Index < 1000;
        ++Index)
    {
        entity *en = CreateEntity(&State->World);
        SetupRock(en, RenderData);
        en->Position = vec2{GetRandomReal32_Range(-WORLD_WIDTH * 4.0f, WORLD_WIDTH * 4.0f), GetRandomReal32_Range(-WORLD_HEIGHT * 4.0f, WORLD_HEIGHT * 4.0f)};    
    }

    sh_glSetClearColor(RenderData, COLOR_TEAL);

    // NOTE(Sleepster): If we ware playing a background track, hot reloading is dead
    //sh_FMODPlaySoundFX(AudioSubsystem->SoundFX[TEST_MUSIC]);
    //sh_PlayBackgroundTrack(SUNKEN_SEA);
}

// NOTE(Sleepster): Make a function that adds to the players speed in the fixed time step to try and fix this weird collision jank
// The idea is that this will add to the velocity here, independant of the framerate so that the player will move the same regardless
// of framerate, but the main frame-indepenent update function will still keep the position correct, potentially fixing our collider
// problems

external
GAME_FIXED_UPDATE(GameFixedUpdate)
{
    // NOTE(Sleepster): For some reason a "for" loop being present within this fixedupdate causes the game to break
    for(uint32 EntityIndex = 0;
        EntityIndex < State->World.EntityCounter;
        ++EntityIndex)
    {
        entity *Temp = &State->World.Entities[EntityIndex];
        if((Temp->Flags & IS_VALID) && (Temp->Arch == PLAYER))
        {
            UpdatePlayerPosition(Temp, State, Time);
            v2Approach(&RenderData->GameCamera.Position, RenderData->GameCamera.Target, 0.003f, Time.DeltaTime);
        }
    }
}

external 
GAME_UPDATE_AND_RENDER(GameUnlockedUpdate)
{
    RenderData->GameCamera.Target = Player->Position;
    RenderData->GameCamera.Zoom = 4.0f;
    RenderData->ViewMatrix = mat4MakeScale(vec3{1.0f, 1.0f, 1.0f});
    RenderData->ViewMatrix = mat4Multiply(RenderData->ViewMatrix, mat4Translate(v2Expand(RenderData->GameCamera.Position, 0.0f)));
    RenderData->ViewMatrix = mat4Multiply(RenderData->ViewMatrix, mat4MakeScale(vec3{1.0f * RenderData->GameCamera.Zoom, 1.0f * RenderData->GameCamera.Zoom, 1.0f}));

    vec2 MousePos = v2Cast(State->GameInput.Keyboard.CurrentMouse);
    DrawUIText(sprints(&Memory->TransientStorage, STR("%f, %f"), MousePos.x, MousePos.y), {0.0f, 200.0f}, 0.05f, COLOR_BLACK, 0, RenderData);

    for(uint32 EntityIndex = 0;
        EntityIndex < State->World.EntityCounter;
        ++EntityIndex)
    {
        entity *en = &State->World.Entities[EntityIndex];
        vec2 Position = en->Position;
        if(en->Flags & IS_VALID)
        {
            switch(en->Arch)
            {
                case PLAYER:
                {
                    DrawEntityStaticSprite2D(en, COLOR_WHITE, 0, RenderData);
                    RenderData->GameCamera.Viewport = {WORLD_WIDTH, WORLD_HEIGHT};
                }break;
                case ROCK:
                {
                    DrawEntityStaticSprite2D(en, COLOR_WHITE, 0, RenderData);
                }break;
            };
            UpdateEntityColliderData(en);
        }
    }
}
