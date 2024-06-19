/* date = June 18th 2024 1:01 pm */

#ifndef _SHIVER__RENDERER_H
#define _SHIVER__RENDERER_H

// STBIMAGE (texture loading)
#define STB_IMAGE_IMPLEMENTATION
#include "stbimage/stb_image.h"

// DEFINES
#define MAX_TRANSFORMS 10000
#define ArraySize(Array, Type) (sizeof(Array) / sizeof(Type))
#define SIMRATE ((1.0f/60.0f)*1000)

#endif //_SHIVER__RENDERER_H
