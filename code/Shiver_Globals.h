/* date = June 25th 2024 6:22 pm */

#ifndef _SHIVER__GLOBALS_H
#define _SHIVER__GLOBALS_H
#include "Shiver_Renderer.h"
#include "Intrinsics.h"
#include "util/Math.h"

// PLATFORM GLOBLALS
global_variable bool GlobalRunning;
global_variable int64 PerfCountFrequency;
global_variable real64 DeltaCounter;
global_variable ivec2 SizeData = {1280, 720};

// ENGINE VARIABLES
global_variable const int32 SAMPLE_RATE = 48000;
global_variable const int32 CHANNEL_COUNT = 2;
global_variable const int32 MAX_SOUNDS = 128;


// SIMULATION GLOBALS
global_variable const int32 MAX_TRANSFORMS = 100000;
global_variable const int32 MAX_UI_TRANSFORMS = 100000;
global_variable const int32 MAX_TEXT_TRANSFORMS = 100000;

global_variable const real32 SIMRATE = ((1.0f/90.0f)*1000);
global_variable const real32 RENDERRATE = ((1.0f/144.0f)*1000);


// GAME GLOBALS
global_variable const int32 WORLD_WIDTH = 320;
global_variable const int32 WORLD_HEIGHT = 180;

global_variable const int32 TILEMAP_SIZE_X = 17;
global_variable const int32 TILEMAP_SIZE_Y = 9;

global_variable const int32 TILESIZE = 16;
global_variable const int32 STATIC_MASS = 10000;

global_variable const real64 GJK_TOLERANCE = 0.001f;
global_variable const real32 EPSILON = 0.05f;

global_variable const int32 MAX_COLLIDER_VERTS = 32;
global_variable const int32 MAX_SIMPLEX_VERTS = 512;
global_variable const int32 MAX_ENTITIES = 1024 * 10;


#endif //_SHIVER__GLOBALS_H
