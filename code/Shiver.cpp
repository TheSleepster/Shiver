#include "Intrinsics.h"
#include "Shiver_Input.h"
#include "Shiver.h"

extern "C"
GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    sh_glCreateStaticSprite2D({0, 0}, {16, 16}, SPRITE_DICE, RenderData);
    sh_glDrawStaticSprite2D(SPRITE_DICE, {100, 100}, RenderData);
}