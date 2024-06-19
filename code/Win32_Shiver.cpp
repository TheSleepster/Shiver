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
#include "Shiver.cpp"

global_variable bool GlobalRunning;


struct FMODEvent
{
    const char *EventPath;
    FMOD_STUDIO_EVENTDESCRIPTION *EventDescription;
    FMOD_STUDIO_EVENTINSTANCE *EventInstance;
};


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
            
            
            // SOUND
            FMOD_SYSTEM *System;
            FMOD_System_Create(&System, FMOD_VERSION);
            
            int32 Test = 10;
            int64 Test2 = 20;
            ShiverArray TestArray = sh_InitArray((void *)&Test, sizeof(Test), 10, &PermanentStorage);
            ShiverArray TestArray2 = sh_InitArray((void *)&Test2, sizeof(Test2), 10, &PermanentStorage);
            
            // TODO(Sleepster): Fix this, currently doesn't work.
            //                  C++ Style array would be great
            int32 Test3 = (int32)TestArray.Data[3];
            
            size_t SizeOfTest1 = sizeof(&TestArray.Data);
            size_t SizeOfTest2 = sizeof(&TestArray2.Data);
            
            sh_ArrayAdd(&TestArray, (char *)&Test);
            int32 Test4 = TestArray.Data[0];
            
            sh_ClearArrayData(&TestArray);
            if(sh_IsIndexNull(&TestArray, 0))
            {
                sh_DeleteArray(&TestArray, &PermanentStorage);
            }
            
            //FMODEvent Events[3];
            
            GlobalRunning = true;
            while(GlobalRunning)
            {
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
                
                GameUpdateAndRender(&RenderData);
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