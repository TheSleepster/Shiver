#include "Intrinsics.h"
#include "Shiver_Input.h"
#include "Shiver.h"

internal void
GameUpdateAndRender(glrenderdata *RenderData)
{
    sh_glCreateStaticSprite2D({0, 0}, {16, 16}, SPRITE_DICE, RenderData);
    
    renderertransform Transform  = {};
    spritedata SpriteInformation = sh_glGetSprite(SPRITE_DICE, RenderData);
    Transform.AtlasOffset = SpriteInformation.AtlasOffset;
    Transform.SpriteSize = SpriteInformation.SpriteSize;
    Transform.WorldPosition = {100.0f, 100.0f};
    Transform.Size = {100.0f, 100.0f};
    RenderData->RendererTransforms[RenderData->TransformCounter++] = Transform;
}