#include "Intrinsics.h"
#include "Shiver_Input.h"
#include "Shiver.h"

#define WORLD_WIDTH 160
#define WORLD_HEIGHT 90

extern "C"
GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    RenderData->Cameras[CAMERA_GAME].Position = {160, -90};
    RenderData->Cameras[CAMERA_GAME].Viewport = {WORLD_WIDTH, WORLD_HEIGHT};
    
    sh_glCreateStaticSprite2D({0, 0}, {16, 16}, SPRITE_DICE, RenderData);
    sh_glDrawStaticSprite2D(SPRITE_DICE, {160, 90}, {16, 16}, RenderData);
}