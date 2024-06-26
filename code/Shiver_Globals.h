/* date = June 25th 2024 6:22 pm */

#ifndef _SHIVER__GLOBALS_H
#define _SHIVER__GLOBALS_H
#include "Intrinsics.h"


// RENDERER GLOBALS
global_variable const int32 MAX_TRANSFORMS = 10000;
global_variable const real32 SIMRATE = ((1.0f/90.0f)*1000);
global_variable const real32 RENDERRATE = ((1.0f/144.0f)*1000);


// PLATFORM GLOBLALS
global_variable bool GlobalRunning;
global_variable int64 PerfCountFrequency;
global_variable real64 DeltaCounter;


// GAME GLOBALS
global_variable const int32 WORLD_WIDTH = 160;
global_variable const int32 WORLD_HEIGHT = 90;

global_variable const int32 TILEMAP_SIZE_X = 17;
global_variable const int32 TILEMAP_SIZE_Y = 9;

global_variable const int32 TILESIZE = 16;
global_variable const int32 STATIC_MASS = 10000;

global_variable const real64 GJK_TOLERANCE = 0.00001;
global_variable const real32 EPSILON = 0.05f;

#endif //_SHIVER__GLOBALS_H
