/* date = June 18th 2024 0:48 pm */

#ifndef _WIN32__SHIVER_H
#define _WIN32__SHIVER_H
#include "Intrinsics.h"
#include "Shiver_Renderer.h"

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

internal inline FILETIME
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

internal inline int32
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

internal inline char *
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

internal inline char *
ReadEntireFileMA(const char *Filepath, uint32 *FileSize, MemoryArena *ArenaAllocator) 
{
    char *File = nullptr; 
    int32 FileSize2 = GetFileSizeInBytes(Filepath);
    Assert(FileSize2 >= 0, "FileSize is less than 0!\n");
    
    char *Buffer = ArenaAlloc(ArenaAllocator, size_t(FileSize2 + 1));
    File = ReadEntireFile(Filepath, FileSize, Buffer);
    
    return(File);
}


#endif //_WIN32__SHIVER_H
