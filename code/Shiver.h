/* date = June 17th 2024 8:58 am */

#ifndef _SHIVER_H
#define _SHIVER_H

#include "Intrinsics.h"
#include "Shiver_Input.h"
#include "Shiver_Renderer.h"

struct gamestate
{
    KeyCodeID KeyCodeLookup[KEY_COUNT];
    Input GameInput;
};

#define GAME_UPDATE_AND_RENDER(name) void name(gamestate *State, glrenderdata *RenderData)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub)
{
}

#endif //_SHIVER_H
