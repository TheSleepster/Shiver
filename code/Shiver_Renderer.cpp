#include "../data/shader/Shiver_SharedShaderHeader.h"
#include "util/MemoryArena.h"
#include "util/Math.h"

// OPENGL
#include "OpenGL/GLL.h"
#include "OpenGL/glcorearb.h"
#include "OpenGL/glext.h"
#include "OpenGL/wglext.h"

// STB IMAGE TEXTURE LOADING
#define STB_IMAGE_IMPLEMENTATION
#include "stbimage/stb_image.h"

// GAME HEADERS
#include "Win32_Shiver.h"
#include "Shiver_Renderer.h"

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