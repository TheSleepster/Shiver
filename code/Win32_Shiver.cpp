#include "Intrinsics.h"

// WINDOWS
#include <windows.h>

// ENGINE UTILS
#include "../data/shader/Shiver_SharedShaderHeader.h"
#include "util/MemoryArena.h"
#include "util/Math.h"
#include "util/ShiverArray.h"

// OPENGL
#include "OpenGL/GLL.h"
#include "OpenGL/glcorearb.h"
#include "OpenGL/glext.h"
#include "OpenGL/wglext.h"

// FMOD 
#include "FMOD/fmod.h"
#include "FMOD/fmod_common.h"

// FMOD STUDIO
#include "FMOD/fmod_studio.h"
#include "FMOD/fmod_studio_common.h"

// GAME HEADERS
#include "Win32_Shiver.h"
#include "Shiver_Renderer.h"

// CPP FILES FOR UNITY BUILD
#include "Shiver_Input.cpp"
#include "Shiver_Renderer.cpp"

global_variable bool GlobalRunning;

internal inline real32 
dBToVolume(real32 dB)
{
    return(powf(10.0f, 0.05f * dB));
}

internal inline real32
VolumeTodB(real32 Volume)
{
    return(20 * log10f(Volume));
}

internal inline FILETIME
Win32MaxFiletime(FILETIME A, FILETIME B) 
{
    if(CompareFileTime(&A, &B) != 0) 
    {
        return(A);
    }
    return(A);
}

internal FILETIME
Win32GetLastWriteTime(const char *Filename) 
{
    FILETIME LastWriteTime = {};
    
    WIN32_FIND_DATA FindData;
    HANDLE FindHandle = FindFirstFileA(Filename, &FindData);
    if(FindHandle != INVALID_HANDLE_VALUE) 
    {
        LastWriteTime = FindData.ftLastWriteTime;
        FindClose(FindHandle);
    }
    
    return(LastWriteTime);
}

// TODO(Sleepster): GetFileSizeInBytes/ReadEntireFile(MA) are platform agnostic
//                  Move them to seperate files later. Doesn't matter rn
internal int32
GetFileSizeInBytes(const char *Filepath) 
{
    int32 FileSize = 0;
    FILE *File = fopen(Filepath, "rb");
    
    fseek(File, 0, SEEK_END);
    FileSize = ftell(File);
    fseek(File, 0, SEEK_SET);
    fclose(File);
    
    return(FileSize);
}

internal char *
ReadEntireFile(const char *Filepath, uint32 *Size, char *Buffer) 
{
    Assert(Filepath != nullptr, "Cannot find the file designated!\n");
    Assert(Buffer != nullptr, "Provide a valid buffer!\n");
    Assert(Size >= 0, "Size is less than 0!\n");
    
    *Size = 0;
    FILE *File = fopen(Filepath, "rb");
    
    fseek(File, 0, SEEK_END);
    *Size = ftell(File);
    fseek(File, 0, SEEK_SET);
    
    memset(Buffer, 0, *Size + 1);
    fread(Buffer, sizeof(char), *Size, File);
    
    fclose(File);
    return(Buffer);
}

internal char *
ReadEntireFileMA(const char *Filepath, uint32 *FileSize, MemoryArena *ArenaAllocator) 
{
    char *File = nullptr; 
    int32 FileSize2 = GetFileSizeInBytes(Filepath);
    Assert(FileSize2 >= 0, "FileSize is less than 0!\n");
    
    char *Buffer = ArenaAlloc(ArenaAllocator, size_t(FileSize2 + 1));
    File = ReadEntireFile(Filepath, FileSize, Buffer);
    
    return(File);
}

// HOT RELOADING CODE

struct win32gamecode
{
    HMODULE GameCodeDLL;
    FILETIME LastWriteTime;
    
    game_update_and_render *UpdateAndRender;
    
    bool IsValid;
    bool IsLoaded;
};

internal win32gamecode
Win32LoadGameCode(const char *SourceDLLName)
{
    win32gamecode Result = {};
    
    char *TempDLLName = "Temp.dll";
    Result.LastWriteTime = Win32GetLastWriteTime(SourceDLLName);
    
    Result.IsLoaded = 0;
    while(!Result.IsLoaded)
    {
        CopyFile(SourceDLLName, TempDLLName, FALSE);
        Result.IsLoaded = true;
    }
    Result.GameCodeDLL = LoadLibraryA(TempDLLName);
    if(Result.GameCodeDLL)
    {
        Result.UpdateAndRender = (game_update_and_render *)
            GetProcAddress(Result.GameCodeDLL, "GameUpdateAndRender");
    }
    else
    {
        Result.UpdateAndRender = GameUpdateAndRenderStub;
    }
    Sleep(100);
    return(Result);
}

internal void
Win32UnloadGameCode(win32gamecode *GameCode)
{
    if(GameCode->GameCodeDLL)
    {
        FreeLibrary(GameCode->GameCodeDLL);
        GameCode->GameCodeDLL = 0;
    }
    GameCode->IsValid = 0;
    GameCode->IsLoaded = 0;
    GameCode->UpdateAndRender = GameUpdateAndRenderStub;
}

LRESULT CALLBACK 
Win32MainWindowCallback(HWND WindowHandle, UINT Message,
                        WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = {};
    
    switch(Message)
    {
        case WM_CLOSE:
        {
            PostQuitMessage(0);
        }
        case WM_DESTROY:
        {
            DestroyWindow(WindowHandle);
            GlobalRunning = false;
        }break;
        default:
        {
            Result = DefWindowProc(WindowHandle, Message, wParam, lParam);
        };
    }
    return(Result);
}

int CALLBACK 
WinMain(HINSTANCE hInstance, 
        HINSTANCE hPrevInstance, 
        LPSTR lpCmdLine, 
        int32 nShowCmd)
{
    win32openglfunctions WGLFunctions = {};
    glrenderdata RenderData = {};
    win32windowdata WindowData = {};
    real32 SimulationDelta = {};
    
    gamestate State = {};
    Win32LoadKeyData(&State);
    
    LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceFrequency(&PerfCountFrequencyResult);
    int64 PerfCountFrequency = PerfCountFrequencyResult.QuadPart;
    
    WNDCLASS Window = {};
    Window.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    Window.lpfnWndProc = Win32MainWindowCallback;
    Window.hInstance = hInstance;
    Window.lpszClassName = "MakeshiftWindow";
    
    if(RegisterClass(&Window))
    {
        Win32InitializeOpenGLFunctionPointers(Window, hInstance, &WGLFunctions);
        // ACTUAL WINDOW USED IN THE PROGRAM
        
        WindowData.SizeData.x = 100;
        WindowData.SizeData.y = 200;
        WindowData.SizeData.Width = 1280;
        WindowData.SizeData.Height = 720;
        
        HWND WindowHandle = 
            CreateWindow(Window.lpszClassName,
                         "Shiver",
                         WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                         WindowData.SizeData.x,
                         WindowData.SizeData.y,
                         WindowData.SizeData.Width,
                         WindowData.SizeData.Height,
                         0,
                         0,
                         hInstance,
                         0);
        if(WindowHandle)
        {
            HDC WindowDC = GetDC(WindowHandle);
            
            LARGE_INTEGER LastCounter;
            QueryPerformanceCounter(&LastCounter);
            
            // MEMORY STUFF
            MemoryArena TransientStorage = MakeMemoryArena(Megabytes(3));
            MemoryArena PermanentStorage = MakeMemoryArena(Megabytes(3));
            // MEMORY STUFF
            
            const int PixelAttributes[] = 
            {
                WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
                WGL_SWAP_METHOD_ARB,    WGL_SWAP_COPY_ARB,
                WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
                WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
                WGL_COLOR_BITS_ARB,     32,
                WGL_ALPHA_BITS_ARB,     8,
                WGL_DEPTH_BITS_ARB,     24,
                0
            };
            
            const int ContextAttributes[] = 
            {
                WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
                WGL_CONTEXT_MINOR_VERSION_ARB, 3,
                WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_DEBUG_BIT_ARB,
                0
            };
            
            UINT NumPixelFormats;
            int32 PixelFormat = 0;
            if(!WGLFunctions.wglChoosePixelFormatARB(WindowDC, PixelAttributes, 0, 1, &PixelFormat, &NumPixelFormats))
            {
                Assert(false, "Failed to choose the Main Pixel Format!\n");
            }
            
            PIXELFORMATDESCRIPTOR MainPixelFormat;
            DescribePixelFormat(WindowDC, PixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &MainPixelFormat);
            SetPixelFormat(WindowDC, PixelFormat, &MainPixelFormat);
            
            HGLRC MainRenderingContext = WGLFunctions.wglCreateContextAttribsARB(WindowDC, 0, ContextAttributes);
            wglMakeCurrent(WindowDC, MainRenderingContext);
            
            // VSYNC
            WGLFunctions.wglSwapIntervalEXT(0);
            // VSYNC
            
            
            InitializeOpenGLRendererData(&RenderData, &TransientStorage);
            win32gamecode Game = Win32LoadGameCode("ShiverGame.dll");
            
            // SOUND
            FMOD_SYSTEM *System;
            FMOD_System_Create(&System, FMOD_VERSION);
            
            struct fmod_event
            {
                const char *EventPath;
                FMOD_STUDIO_EVENTDESCRIPTION *EventDescription;
                FMOD_STUDIO_EVENTINSTANCE *EventInstance;
            };
            
            fmod_event Event = {};
            ShiverArray Events = sh_InitArray((void *)&Event, sizeof(fmod_event), 10, &PermanentStorage);
            
            
            
            GlobalRunning = true;
            while(GlobalRunning)
            {
#if SHIVER_SLOW
                const char *SourceDLLName = "ShiverGame.dll";
                FILETIME NewDLLWriteTime = Win32GetLastWriteTime(SourceDLLName);
                if(CompareFileTime(&NewDLLWriteTime, &Game.LastWriteTime) != 0)
                {
                    Win32UnloadGameCode(&Game);
                    Game = Win32LoadGameCode(SourceDLLName);
                }
#endif
                
                
                MSG Message = {0};
                while(PeekMessageA(&Message, WindowHandle, 0, 0, PM_REMOVE))
                {
                    switch(Message.message)
                    {
                        case WM_SIZE:
                        {
                            RECT Rect  = {};
                            GetClientRect(WindowHandle, &Rect);
                            WindowData.SizeData.Width = Rect.right - Rect.left;
                            WindowData.SizeData.Height = Rect.top - Rect.bottom;
                        }break;
                        
                        case WM_SYSKEYDOWN:
                        case WM_SYSKEYUP:
                        case WM_KEYDOWN: 
                        case WM_KEYUP:
                        {
                            uint32 VKCode = (uint32)Message.wParam;
                            bool WasDown = ((Message.lParam & (1 << 30)) != 0);
                            bool IsDown = ((Message.lParam & (1 << 31)) == 0);
                            
                            KeyCodeID KeyCode = State.KeyCodeLookup[Message.wParam];
                            Key *Key = &State.GameInput.Keyboard.Keys[KeyCode];
                            Key->JustPressed = !Key->JustPressed && !Key->IsDown && IsDown;
                            Key->JustReleased = !Key->JustReleased && Key->IsDown && !IsDown;
                            Key->IsDown = IsDown;
                            
                            
                            bool AltKeyIsDown = ((Message.lParam & (1 << 29)) !=0);
                            if(VKCode == VK_F4 && AltKeyIsDown) 
                            {
                                GlobalRunning = false;
                            }
                        }break;
                        case WM_LBUTTONDOWN:
                        case WM_RBUTTONDOWN:
                        case WM_MBUTTONDOWN:
                        case WM_XBUTTONDOWN:
                        case WM_LBUTTONUP:
                        case WM_RBUTTONUP:
                        case WM_MBUTTONUP: 
                        case WM_XBUTTONUP:
                        {
                            uint32 VKCode = (uint32)Message.wParam;
                            bool IsDown = (GetKeyState(VKCode) & (1 << 15));
                            
                            KeyCodeID KeyCode = State.KeyCodeLookup[Message.wParam];
                            Key *Key = &State.GameInput.Keyboard.Keys[KeyCode];
                            Key->JustPressed = !Key->JustPressed && !Key->IsDown && IsDown;
                            Key->JustReleased = !Key->JustReleased && Key->IsDown && !IsDown;
                            Key->IsDown = IsDown;
                        }break;
                        
                        default:
                        {
                            TranslateMessage(&Message);
                            DispatchMessage(&Message);
                        }break;
                    }
                }
                
                // DELTA TIME
                if(SimulationDelta >= SIMRATE)
                {
                    if(SimulationDelta >= (SIMRATE*2))
                    {
                        SimulationDelta = SIMRATE;
                    }
                    real32 InterpolationDelta = SimulationDelta / SIMRATE;
                }
                
                Game.UpdateAndRender(&State, &RenderData);
                sh_glRender(&WindowData, WindowHandle, &RenderData, &TransientStorage);
                
                RenderData.TransformCounter = 0;
                TransientStorage.Used = 0;
                
                LARGE_INTEGER EndCounter;
                QueryPerformanceCounter(&EndCounter);
                int64 DeltaCounter = EndCounter.QuadPart - LastCounter.QuadPart;
                real32 MSPerFrame = (1000 *(real32)DeltaCounter) / real32(PerfCountFrequency);
                int32 FPS = int32(PerfCountFrequency / DeltaCounter);
                
                SimulationDelta += MSPerFrame;
                LastCounter = EndCounter;
                
                print_m("%.02fms\n", MSPerFrame);
                print_m("FPS: %d\n", FPS);
            }
        }
        else
        {
            Assert(false, "Failed to create the window handle\n");
        }
    }
    else
    {
        Assert(false, "Failed to create the window class\n");
    }
    return(0);
}