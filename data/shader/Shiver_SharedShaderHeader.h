/* date = June 17th 2024 3:14 pm */

#ifndef _SHIVER__SHARED_SHADER_HEADER_H
#define _SHIVER__SHARED_SHADER_HEADER_H

#ifdef ENGINE // ENGINE ONLY
#include "Intrinsics.h"
#include "util/Math.h"
#include "util/MemoryArena.h"

#else // SHADER ONLY
#endif

// SHARED
struct renderertransform
{
    ivec2 AtlasOffset;
    ivec2 SpriteSize;
    vec2 WorldPosition;
    vec2 Size;
};

#endif //_SHIVER__SHARED_SHADER_HEADER_H
