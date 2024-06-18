#include "Intrinsics.h"

#include <windows.h>

#include "FMOD/fmod.h"
#include "FMOD/fmod_common.h"
#include "FMOD/fmod_studio.h"

#include "../data/shader/Shiver_SharedShaderHeader.h"
#include "util/MemoryArena.h"
#include "util/Math.h"

#include "OpenGL/GLL.h"

#include "OpenGL/glcorearb.h"
#include "OpenGL/glext.h"
#include "OpenGL/wglext.h"

#define MAX_TRANSFORMS 10000
#define ArraySize(Array, Type) (sizeof(Array) / sizeof(Type))
#define SIMRATE ((1.0f/60.0f)*1000)
#define STB_IMAGE_IMPLEMENTATION

#include "stbimage/stb_image.h"

#include "Shiver_Input.cpp"
#include "Shiver.cpp"

global_variable bool GlobalRunning;

enum ShaderPrograms
{
    BASIC,
    SHADERCOUNT
};

enum GlRendererTextures
{
    GAME_ATLAS,
    TEXTURE_COUNT
};

enum sprites
{
    SPRITE_DICE,
    SPRITE_COUNT
};

struct win32windowdata
{
    ivec4 SizeData;
};

struct texture2d
{
    ivec3 TextureData;
    GLenum ActiveTexture;
    char *RawData;
    
    GLuint TextureID;
    
    FILETIME LastTimeStamp;
};

struct glshaderprogram
{
    GLuint ShaderID;
    FILETIME LastTimeStamp;
};

struct shader
{
    glshaderprogram VertexShader;
    glshaderprogram FragmentShader;
    GLuint Shader;
};

struct spritedata
{
    ivec2 AtlasOffset;
    ivec2 SpriteSize;
};

struct glrenderdata
{
    uint32 TransformCounter;
    renderertransform RendererTransforms[MAX_TRANSFORMS];
    
    GLuint RendererTransformsSBOID;
    GLuint ScreenSizeID;
    GLuint OrthographicMatrixID;
    
    texture2d Textures[31];
    shader Shaders[1];
    
    spritedata Sprites[20];
};

struct win32openglfunctions
{
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
};

internal inline FILETIME
maxFiletime(FILETIME A, FILETIME B) 
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
    
    char *Buffer = BumpAllocate(ArenaAllocator, size_t(FileSize2 + 1));
    File = ReadEntireFile(Filepath, FileSize, Buffer);
    
    return(File);
}

// OPENGL STUFF

internal void APIENTRY
OpenGLDebugMessageCallback(GLenum Source, GLenum Type, GLuint ID, GLenum Severity,
                           GLsizei Length, const GLchar *Message, const void *UserParam)
{
    if(Severity == GL_DEBUG_SEVERITY_LOW||
       Severity == GL_DEBUG_SEVERITY_MEDIUM||
       Severity == GL_DEBUG_SEVERITY_HIGH)
    {
        print_m("Error: %s\n", Message);
        Assert(false, "STOPPING\n");
    }
    else
    {
        print_m("Warning: %s\n", Message);
    }
}

internal void
sh_glVerifyivSuccess(GLuint TestID, GLuint Type)
{
    bool32 Success = {};
    char ShaderLog[512] = {};
    
    switch(Type)
    {
        case GL_VERTEX_SHADER:
        case GL_FRAGMENT_SHADER:
        {
            glGetShaderiv(TestID, GL_COMPILE_STATUS, &Success);
        }break;
        case GL_PROGRAM:
        {
            glGetProgramiv(TestID, GL_LINK_STATUS, &Success);
        }break;
    }
    if(!Success)
    {
        glGetShaderInfoLog(TestID, 512, 0, ShaderLog);
        print_m("ERROR ON SHADER: %s\n", ShaderLog);
        Assert(false, "STOPPING\n");
    }
}

internal void
sh_glCreateAndLoadTexture(const char *Filepath, texture2d TextureInfo)
{
    glActiveTexture(TextureInfo.ActiveTexture);
    TextureInfo.RawData = 
    (char *)stbi_load(Filepath, &TextureInfo.TextureData.Width, &TextureInfo.TextureData.Height, &TextureInfo.TextureData.Channels, 4);
    
    if(TextureInfo.RawData)
    {
        TextureInfo.LastTimeStamp = Win32GetLastWriteTime(Filepath);
        
        glGenTextures(0, &TextureInfo.TextureID);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, 
                     TextureInfo.TextureData.Width, TextureInfo.TextureData.Height, 0, 
                     GL_RGBA, GL_UNSIGNED_BYTE, TextureInfo.RawData);
    }
    stbi_image_free(TextureInfo.RawData);
}

internal glshaderprogram
sh_glCreateShader(int32 ShaderType, const char *Filepath, MemoryArena *Memory)
{
    uint32 FileSize = 0;
    glshaderprogram ReturnShader = {};
    
    char *ShaderHeader = ReadEntireFileMA("shader/Shiver_SharedShaderHeader.h", &FileSize, Memory);
    char *ShaderSource = ReadEntireFileMA(Filepath, &FileSize, Memory);
    if(ShaderSource && ShaderHeader)
    {
        char *ShaderSources[] = 
        {
            "#version 430 core\n",
            ShaderHeader,
            ShaderSource
        };
        
        ReturnShader.ShaderID = glCreateShader(ShaderType);
        glShaderSource(ReturnShader.ShaderID, ArraySize(ShaderSources, char *), ShaderSources, 0);
        glCompileShader(ReturnShader.ShaderID);
        sh_glVerifyivSuccess(ReturnShader.ShaderID, GL_VERTEX_SHADER);
        
        ReturnShader.LastTimeStamp = Win32GetLastWriteTime(Filepath);
    }
    else
    {
        Trace("File is either not found or does not contain strings!\n");
        Assert(false, "STOPPING\n");
    }
    return(ReturnShader);
}

internal shader
sh_glCreateProgram(glshaderprogram VertexShaderID, glshaderprogram FragmentShaderID)
{
    shader ReturnProgram;
    ReturnProgram.Shader = glCreateProgram();
    
    glAttachShader(ReturnProgram.Shader, VertexShaderID.ShaderID);
    glAttachShader(ReturnProgram.Shader, FragmentShaderID.ShaderID);
    glLinkProgram(ReturnProgram.Shader);
    
    sh_glVerifyivSuccess(ReturnProgram.Shader, GL_PROGRAM);
    
    glDetachShader(ReturnProgram.Shader, VertexShaderID.ShaderID);
    glDetachShader(ReturnProgram.Shader, FragmentShaderID.ShaderID);
    glDeleteShader(VertexShaderID.ShaderID);
    glDeleteShader(FragmentShaderID.ShaderID);
    
    return(ReturnProgram);
}

internal inline void
sh_glCreateStaticSprite2D(ivec2 AtlasOffset, ivec2 SpriteSize, 
                          sprites SpriteID, glrenderdata *RenderData)
{
    spritedata Result = {};
    
    Result.AtlasOffset = AtlasOffset;
    Result.SpriteSize = SpriteSize;
    
    RenderData->Sprites[SpriteID] = Result;
}

internal inline spritedata
sh_glGetSprite(sprites SpriteID, glrenderdata *RenderData)
{
    spritedata Result = RenderData->Sprites[SpriteID];
    return(Result);
}

internal void
Win32InitializeOpenGLFunctionPointers(WNDCLASS Window, HINSTANCE hInstance, 
                                      win32openglfunctions *WGLFunctions)
{
    HWND DummyWindow = 
        CreateWindow(Window.lpszClassName,
                     "OpenGLFunctionGetter",
                     WS_OVERLAPPEDWINDOW,
                     CW_USEDEFAULT,
                     CW_USEDEFAULT,
                     CW_USEDEFAULT,
                     CW_USEDEFAULT,
                     0,
                     0,
                     hInstance,
                     0);
    HDC DummyContext = GetDC(DummyWindow);
    
    PIXELFORMATDESCRIPTOR DFormat = {};
    DFormat.nSize = sizeof(DFormat);
    DFormat.nVersion = 1;
    DFormat.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
    DFormat.iPixelType = PFD_TYPE_RGBA;
    DFormat.cColorBits = 32;
    DFormat.cAlphaBits = 8;
    DFormat.cDepthBits = 24;
    
    int32 dPixelFormat = ChoosePixelFormat(DummyContext, &DFormat);
    PIXELFORMATDESCRIPTOR DSuggestedFormat;
    DescribePixelFormat(DummyContext, dPixelFormat, sizeof(DSuggestedFormat), &DSuggestedFormat);
    SetPixelFormat(DummyContext, dPixelFormat,  &DSuggestedFormat);
    
    HGLRC TempRC = wglCreateContext(DummyContext);
    wglMakeCurrent(DummyContext, TempRC);
    
    WGLFunctions->wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)
        wglGetProcAddress("wglChoosePixelFormatARB");
    WGLFunctions->wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)
        wglGetProcAddress("wglCreateContextAttribsARB");
    WGLFunctions->wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)
        wglGetProcAddress("wglSwapIntervalEXT");
    if(!WGLFunctions->wglChoosePixelFormatARB||
       !WGLFunctions->wglCreateContextAttribsARB||
       !WGLFunctions->wglSwapIntervalEXT)
    {
        Assert(false, "Failed to extract OpenGL function Pointers!\n");
    }
    
    wglMakeCurrent(DummyContext, 0);
    wglDeleteContext(TempRC);
    ReleaseDC(DummyWindow, DummyContext);
    DestroyWindow(DummyWindow);
}

internal void
InitializeOpenGLRendererData(glrenderdata *RenderData, MemoryArena *TransientStorage)
{
    LoadOpenGLFunctions();
    
    glDebugMessageCallback(&OpenGLDebugMessageCallback, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);
    
    // Shader Setup
    {
        RenderData->Shaders[BASIC].VertexShader = 
            sh_glCreateShader(GL_VERTEX_SHADER, "shader/Basic.vert", TransientStorage);
        
        RenderData->Shaders[BASIC].FragmentShader = 
            sh_glCreateShader(GL_FRAGMENT_SHADER, "shader/Basic.frag", TransientStorage);
        
        RenderData->Shaders[BASIC] = 
            sh_glCreateProgram(RenderData->Shaders[BASIC].VertexShader, RenderData->Shaders[BASIC].FragmentShader);
    }
    
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &RenderData->RendererTransformsSBOID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, RenderData->RendererTransformsSBOID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(renderertransform) * MAX_TRANSFORMS, 
                 RenderData->RendererTransforms, GL_DYNAMIC_DRAW);
    
    RenderData->Textures[GAME_ATLAS].ActiveTexture = GL_TEXTURE0;
    sh_glCreateAndLoadTexture("res/textures/TextureAtlas.png", RenderData->Textures[GAME_ATLAS]);
    
    glEnable(GL_FRAMEBUFFER_SRGB);
    glDisable(0x809D); // Disabling multisampling
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);
    
    glUseProgram(RenderData->Shaders[BASIC].Shader);
}

internal void
sh_glRender(win32windowdata *WindowData, HWND WindowHandle, glrenderdata *RenderData, MemoryArena *Memory)
{
    HDC WindowDC = GetDC(WindowHandle);
    glClearColor(0.3f, 0.1f, 1.0f, 1.0f);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    vec2 WindowSize = {(real32)WindowData->SizeData.Width, (real32)WindowData->SizeData.Height};
    glViewport(0, 0, WindowData->SizeData.Width, WindowData->SizeData.Height);
    
    glUniform2fv(RenderData->ScreenSizeID, 1, &WindowSize.x);
    
    RenderData->ScreenSizeID = 
        glGetUniformLocation(RenderData->Shaders[BASIC].Shader, "ScreenSize");
    
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(renderertransform) * RenderData->TransformCounter, RenderData->RendererTransforms);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, RenderData->TransformCounter);
    SwapBuffers(WindowDC);
}


// END OF OPENGL

LRESULT CALLBACK 
Win32MainWindowCallback(HWND WindowHandle, UINT Message,
                        WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = {};
    
    switch(Message)
    {
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
                        case WM_CLOSE:
                        {
                            PostQuitMessage(0);
                        }
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
                
                sh_glCreateStaticSprite2D({0, 0}, {16, 16}, SPRITE_DICE, &RenderData);
                
                renderertransform Transform  = {};
                spritedata SpriteInformation = sh_glGetSprite(SPRITE_DICE, &RenderData);
                Transform.AtlasOffset = SpriteInformation.AtlasOffset;
                Transform.SpriteSize = SpriteInformation.SpriteSize;
                Transform.WorldPosition = {100.0f, 100.0f};
                Transform.Size = {100.0f, 100.0f};
                RenderData.RendererTransforms[RenderData.TransformCounter++] = Transform;
                
                
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