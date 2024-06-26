/* date = June 18th 2024 0:48 pm */

#ifndef _WIN32__SHIVER_H
#define _WIN32__SHIVER_H
#include "Intrinsics.h"
#include "Shiver_Renderer.h"
#include "Shiver.h"

struct win32openglfunctions
{
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
};

struct win32gamecode
{
    HMODULE GameCodeDLL;
    FILETIME LastWriteTime;
    
    game_on_awake *OnAwake;
    game_fixed_update *FixedUpdate;
    game_update_and_render *UnlockedUpdate;
    
    bool IsValid;
    bool IsLoaded;
};

internal FILETIME Win32MaxFileTime(FILETIME A, FILETIME B);
internal FILETIME Win32GetLastWriteTime(const char *Filename);
internal int32 GetFileSizeInBytes(const char *Filepath);
internal char *ReadEntireFile(const char *Filepath, uint32 *Size, char *Buffer);
internal char *ReadEntireFileMA(const char *Filepath, uint32 *FileSize, MemoryArena *ArenaAllocator);

#endif //_WIN32__SHIVER_H