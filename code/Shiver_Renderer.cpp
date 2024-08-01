#include "../data/shader/Shiver_SharedShaderHeader.h"
#include "Intrinsics.h"
#include "Shiver_Globals.h"
#include "util/MemoryArena.h"
#include "util/Math.h"
#include "Shiver.h"
// OPENGL

#if 0
#include "../data/deps/OpenGL/GLL.h"
#include "../data/deps/OpenGL/glcorearb.h"
#include "../data/deps/OpenGL/glext.h"
#include "../data/deps/OpenGL/wglext.h"
#endif

// STB IMAGE TEXTURE LOADING
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../data/deps/stbimage/stb_image.h"
#include "../data/deps/stbimage/stb_image_write.h"

// GAME HEADERS
#include "Win32_Shiver.h"

// FONT RENDERING
#define MAX_FONT_SIZE 512
#define BITMAP_ATLAS_SIZE 1024

internal void
sh_glLoadFont(char *Filepath, int32 Size, glrenderdata *RenderData, MemoryArena *Scratch)
{
    fontdata Font = {};
    Font.FontSize = Size;
    FT_Error Error;

    Error = FT_Init_FreeType(&Font.FontLib);
    if(Error)
    {
        printm("Failure to Initialize the font lib!: %d", Error);
        dAssert(false);
    }

    Error = FT_New_Face(Font.FontLib, Filepath, 0, &Font.FontFace);
    if(Error == FT_Err_Unknown_File_Format)
    {
        printm("The font file could be opened, but it's file format is not supported!: %d\n", Error);
        dAssert(false);
    }
    else if(Error)
    {
        printm("Failed to load the font file!: %d\n", Error);
        dAssert(false);
    }

    Error = FT_Set_Pixel_Sizes(Font.FontFace, 0, Font.FontSize);
    if(Error)
    {
        printm("Issue setting the pixel Size of the font!: %d\n", Error);
        dAssert(false);
    }

    Font.AtlasPadding = 2;
    int32 Row = 0;
    int32 Column = Font.AtlasPadding;
    
    char *TextureBuffer = ArenaAlloc(Scratch, (uint64)(sizeof(char) * (BITMAP_ATLAS_SIZE * BITMAP_ATLAS_SIZE)));
    
    // FONT LOADING
    for(uint8 GlyphIndex = 32; 
        GlyphIndex < 127;
        ++GlyphIndex)
    {
        FT_UInt Glyph = FT_Load_Char(Font.FontFace, GlyphIndex, FT_LOAD_RENDER); 
        if(Error)
        {
            printm("Failed to render the glyph!: %d\n", Error);
            dAssert(false);
        }

        if(Column + Font.FontFace->glyph->bitmap.width + Font.AtlasPadding >= BITMAP_ATLAS_SIZE)
        {
            Column = Font.AtlasPadding;
            Row += Font.FontSize;
        }

        RenderData->FontHeight = Max((Font.FontFace->size->metrics.ascender - Font.FontFace->size->metrics.descender) >> 6, 
                               RenderData->FontHeight);
        for(uint32 YIndex = 0;
            YIndex < Font.FontFace->glyph->bitmap.rows;
            ++YIndex)
        {
            for(uint32 XIndex = 0;
                XIndex < Font.FontFace->glyph->bitmap.width;
                ++XIndex)
            {
                TextureBuffer[(Row + YIndex) * BITMAP_ATLAS_SIZE + Column + XIndex] = 
                    Font.FontFace->glyph->bitmap.buffer[YIndex * Font.FontFace->glyph->bitmap.width + XIndex];
            }
        }

        glyph *FontGlyph = &RenderData->Glyphs[GlyphIndex];
        FontGlyph->uv = {Column, Row};
        FontGlyph->GlyphSize = 
        {
            (int32)Font.FontFace->glyph->bitmap.width, 
            (int32)Font.FontFace->glyph->bitmap.rows
        };
        FontGlyph->Advance = 
        {
            real32(Font.FontFace->glyph->advance.x >> 6), 
            real32(Font.FontFace->glyph->advance.y >> 6)
        };
        FontGlyph->Offset = 
        {
            real32(Font.FontFace->glyph->bitmap_left),
            real32(Font.FontFace->glyph->bitmap_top)
        };

        Column += Font.FontFace->glyph->bitmap.width + Font.AtlasPadding;
    }

    FT_Done_Face(Font.FontFace);
    FT_Done_FreeType(Font.FontLib);

    // OPENGL TEXTURE
    {
        glGenTextures(1, (GLuint *)&RenderData->LM_FontAtlasID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, RenderData->LM_FontAtlasID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, BITMAP_ATLAS_SIZE, BITMAP_ATLAS_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, (char *)TextureBuffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
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

internal void APIENTRY
OpenGLDebugMessageCallback(GLenum Source, GLenum Type, GLuint ID, GLenum Severity,
                           GLsizei Length, const GLchar *Message, const void *UserParam)
{
    if(Severity == GL_DEBUG_SEVERITY_LOW||
       Severity == GL_DEBUG_SEVERITY_MEDIUM||
       Severity == GL_DEBUG_SEVERITY_HIGH)
    {
        printm("Error: %s\n", Message);
        Assert(false, "STOPPING\n");
    }
    else
    {
        printm("Warning: %s\n", Message);
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
        printm("ERROR ON SHADER: %s\n", ShaderLog);
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

internal void
sh_glReloadExistingTexture(const char *Filepath, texture2d TextureInfo)
{
    glActiveTexture(TextureInfo.ActiveTexture);
    TextureInfo.RawData = 
    (char *)stbi_load(Filepath, &TextureInfo.TextureData.Width, &TextureInfo.TextureData.Height, &TextureInfo.TextureData.Channels, 4);
    
    if(TextureInfo.RawData)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, 
                     TextureInfo.TextureData.Width, TextureInfo.TextureData.Height, 0, 
                     GL_RGBA, GL_UNSIGNED_BYTE, TextureInfo.RawData);
    }
    stbi_image_free(TextureInfo.RawData);
}



internal glshaderprogram
sh_glCreateShader(int32 ShaderType, char *Filepath, MemoryArena *Scratch)
{
    uint32 FileSize = 0;
    glshaderprogram ReturnShader = {};
    
    char *ShaderHeader = ReadEntireFileMA("shader/Shiver_SharedShaderHeader.h", &FileSize, Scratch);
    char *ShaderSource = ReadEntireFileMA(Filepath, &FileSize, Scratch);
    
    ReturnShader.Filepath = Filepath;
    ReturnShader.LastWriteTime = Win32GetLastWriteTime(Filepath); 
    
    if(ShaderSource && ShaderHeader)
    {
        char *ShaderSources[] = 
        {
            "#version 430 core\n",
            ShaderHeader,
            ShaderSource
        };
        
        ReturnShader.ShaderID = glCreateShader(ShaderType);
        glShaderSource(ReturnShader.ShaderID, ArrayCount(ShaderSources), ShaderSources, 0);
        glCompileShader(ReturnShader.ShaderID);
        sh_glVerifyivSuccess(ReturnShader.ShaderID, GL_VERTEX_SHADER);
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
    
    ReturnProgram.VertexShader = VertexShaderID;
    ReturnProgram.FragmentShader = FragmentShaderID;
    
    return(ReturnProgram);
}

internal void
InitializeOpenGLRendererData(glrenderdata *RenderData, MemoryArena *TransientStorage)
{
    LoadOpenGLFunctions();
    
    glDebugMessageCallback(&OpenGLDebugMessageCallback, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);
    
    /////////////////////////////
    // NOTE(Sleepster): Shader setup
    {
        RenderData->Shaders[BASIC].VertexShader.Filepath = "shader/Basic_vert.glsl";
        RenderData->Shaders[BASIC].FragmentShader.Filepath = "shader/Basic_frag.glsl";
        
        RenderData->Shaders[BASIC].VertexShader = 
            sh_glCreateShader(GL_VERTEX_SHADER, RenderData->Shaders[BASIC].VertexShader.Filepath, TransientStorage);
        
        RenderData->Shaders[BASIC].FragmentShader = 
            sh_glCreateShader(GL_FRAGMENT_SHADER, RenderData->Shaders[BASIC].FragmentShader.Filepath, TransientStorage);
        
        RenderData->Shaders[BASIC] = 
            sh_glCreateProgram(RenderData->Shaders[BASIC].VertexShader, RenderData->Shaders[BASIC].FragmentShader);
    }
    
    RenderData->ScreenSizeID = 
        glGetUniformLocation(RenderData->Shaders[BASIC].Shader, "ScreenSize");
    RenderData->OrthographicMatrixID = 
        glGetUniformLocation(RenderData->Shaders[BASIC].Shader, "ProjectionMatrix");
    RenderData->ViewMatrixID = 
        glGetUniformLocation(RenderData->Shaders[BASIC].Shader, "ViewMatrix");
    
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &RenderData->RendererTransformsSBOID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, RenderData->RendererTransformsSBOID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(renderertransform) * MAX_TRANSFORMS, 
                 RenderData->RendererTransforms, GL_DYNAMIC_DRAW);
    
    RenderData->Textures[TEXTURE_GAME_ATLAS].Filepath = "res/textures/TextureAtlas.png";
    RenderData->Textures[TEXTURE_GAME_ATLAS].ActiveTexture = GL_TEXTURE0;
    sh_glCreateAndLoadTexture(RenderData->Textures[TEXTURE_GAME_ATLAS].Filepath, RenderData->Textures[TEXTURE_GAME_ATLAS]);
    
    RenderData->Textures[TEXTURE_GAME_ATLAS].LastWriteTime = Win32GetLastWriteTime(RenderData->Textures[TEXTURE_GAME_ATLAS].Filepath);

    // NOTE(Sleepster): Font Atlas
    sh_glLoadFont("res/fonts/LiberationMono-Regular.ttf", 96, RenderData, TransientStorage);
    
    RenderData->Shaders[BASIC].VertexShader.LastWriteTime = 
        Win32GetLastWriteTime(RenderData->Shaders[BASIC].VertexShader.Filepath);
    
    RenderData->Shaders[BASIC].FragmentShader.LastWriteTime = 
        Win32GetLastWriteTime(RenderData->Shaders[BASIC].FragmentShader.Filepath);
    
    RenderData->GameCamera.Position = {0.0f, 0.0f};
    RenderData->GameCamera.Viewport = {WORLD_WIDTH, WORLD_HEIGHT};

    RenderData->UICamera.Position = {0.0f, 0.0f};
    RenderData->UICamera.Viewport = {WORLD_WIDTH, WORLD_HEIGHT};

    RenderData->ViewMatrix = mat4MatrixFromScaler(1.0f);

    glEnable(GL_FRAMEBUFFER_SRGB);
    glDisable(0x809D); // Disabling multisampling
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);

    sh_glSetClearColor(RenderData, COLOR_TEAL);
    glUseProgram(RenderData->Shaders[BASIC].Shader);
}

internal void
sh_glRender(win32windowdata *WindowData, HWND WindowHandle, glrenderdata *RenderData, MemoryArena *Memory)
{
    //////////////////////
    // NOTE(Sleepster): Shader and texture reloading
#if SHIVER_SLOW
    FILETIME NewTextureWriteTime = Win32GetLastWriteTime(RenderData->Textures[TEXTURE_GAME_ATLAS].Filepath);
    FILETIME NewVertexShaderWriteTime = Win32GetLastWriteTime(RenderData->Shaders[BASIC].VertexShader.Filepath);
    FILETIME NewFragmentShaderWriteTime = Win32GetLastWriteTime(RenderData->Shaders[BASIC].FragmentShader.Filepath);
    
    if(CompareFileTime(&NewTextureWriteTime, &RenderData->Textures[TEXTURE_GAME_ATLAS].LastWriteTime) != 0)
    {
        sh_glReloadExistingTexture(RenderData->Textures[TEXTURE_GAME_ATLAS].Filepath, RenderData->Textures[TEXTURE_GAME_ATLAS]);
        RenderData->Textures[TEXTURE_GAME_ATLAS].LastWriteTime = NewTextureWriteTime;
        Sleep(100);
    }
    
    if(CompareFileTime(&NewVertexShaderWriteTime, &RenderData->Shaders[BASIC].VertexShader.LastWriteTime) != 0||
       CompareFileTime(&NewFragmentShaderWriteTime, &RenderData->Shaders[BASIC].FragmentShader.LastWriteTime) != 0)
    {
        RenderData->Shaders[BASIC].VertexShader = 
            sh_glCreateShader(GL_VERTEX_SHADER, RenderData->Shaders[BASIC].VertexShader.Filepath, Memory);
        
        RenderData->Shaders[BASIC].FragmentShader = 
            sh_glCreateShader(GL_FRAGMENT_SHADER, RenderData->Shaders[BASIC].FragmentShader.Filepath, Memory);
        
        glDeleteProgram(RenderData->Shaders[BASIC].Shader);

        RenderData->Shaders[BASIC] = 
            sh_glCreateProgram(RenderData->Shaders[BASIC].VertexShader, RenderData->Shaders[BASIC].FragmentShader);

        glUseProgram(RenderData->Shaders[BASIC].Shader);
        Sleep(100);
    }
#endif
    glClearColor(RenderData->ClearColor.r, RenderData->ClearColor.g, RenderData->ClearColor.b, RenderData->ClearColor.a);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, WindowData->SizeData.Width, WindowData->SizeData.Height);
    
    // NOTE(Sleepster): Game Rendering Pass
    {
        vec4 CameraInfo = 
        {
            WindowData->SizeData.Width  * -0.5f,
            WindowData->SizeData.Width  *  0.5f,
            WindowData->SizeData.Height * -0.5f,
            WindowData->SizeData.Height *  0.5f,
        };

        RenderData->GameCamera.CameraMatrix = CreateOrthographicMatrix(CameraInfo);
        glUniformMatrix4fv(RenderData->OrthographicMatrixID, 1, GL_FALSE, (const GLfloat *)&RenderData->GameCamera.CameraMatrix.Elements[0][0]);
        glUniformMatrix4fv(RenderData->ViewMatrixID, 1, GL_FALSE, (const GLfloat *)&RenderData->ViewMatrix.Elements[0][0]);

        vec2 ScreenSize = vec2{(real32)WindowData->SizeData.Width, (real32)WindowData->SizeData.Height};
        glUniform2fv(RenderData->ScreenSizeID, 1, &ScreenSize.x);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(renderertransform) * RenderData->TransformCounter, RenderData->RendererTransforms);

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, RenderData->TransformCounter);
    }

    // NOTE(Sleepster): Game Text rendering Pass
    {
        glUniformMatrix4fv(RenderData->OrthographicMatrixID, 1, GL_FALSE, (const GLfloat *)&RenderData->GameCamera.CameraMatrix.Elements[0][0]);
        glUniformMatrix4fv(RenderData->ViewMatrixID, 1, GL_FALSE, (const GLfloat *)&RenderData->ViewMatrix.Elements[0][0]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(renderertransform) * RenderData->GameTextTransformCounter, RenderData->GameTextTransforms);

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, RenderData->GameTextTransformCounter);
    }

    // NOTE(Sleepster): UI rendering Pass
    {
        vec4 CameraInfo = 
        {
            WindowData->SizeData.Width  * -0.5f,
            WindowData->SizeData.Width  *  0.5f,
            WindowData->SizeData.Height * -0.5f,
            WindowData->SizeData.Height *  0.5f,
        };

        RenderData->UICamera.CameraMatrix = CreateOrthographicMatrix(CameraInfo);
        glUniformMatrix4fv(RenderData->OrthographicMatrixID, 1, GL_FALSE, (const GLfloat *)&RenderData->UICamera.CameraMatrix.Elements[0][0]);
        glUniformMatrix4fv(RenderData->ViewMatrixID, 1, GL_FALSE, (const GLfloat *)&RenderData->ViewMatrix.Elements[0][0]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(renderertransform) * RenderData->UITransformCounter, RenderData->UITransforms);

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, RenderData->UITransformCounter);
    }
}
