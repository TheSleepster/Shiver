/* date = June 18th 2024 1:01 pm */

#ifndef _SHIVER__RENDERER_H
#define _SHIVER__RENDERER_H

#include "util/Math.h"

// COLORS
global_variable const vec4 COLOR_WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
global_variable const vec4 COLOR_BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
global_variable const vec4 COLOR_RED = {1.0f, 0.0f, 0.0f, 1.0f};
global_variable const vec4 COLOR_GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
global_variable const vec4 COLOR_BLUE = {0.0f, 0.0f, 1.0f, 1.0f};
global_variable const vec4 COLOR_TEAL = {0.1f, 0.6f, 1.0f, 1.0f};

#include "../data/shader/Shiver_SharedShaderHeader.h"
#include "util/MemoryArena.h"

#include "Shiver_Globals.h"
#include "../data/deps/OpenGL/GLL.h"


enum ShaderPrograms
{
    BASIC,
    BLOOM,
    SHADERCOUNT
};

enum GlRendererTextures
{
    NULL_TEXTURE,
    TEXTURE_GAME_ATLAS,
    
    TEXTURE_COUNT
};

struct win32windowdata
{
    ivec4 SizeData;
};

struct texture2d
{
    FILETIME LastWriteTime;

    ivec3 TextureData;
    char *RawData;
    char *Filepath;
    
    GLuint TextureID;
    GLenum ActiveTexture;
};

struct glshaderprogram
{
    GLuint ShaderID;
    char *Filepath;
    FILETIME LastWriteTime;
};

struct shader
{
    glshaderprogram VertexShader;
    glshaderprogram FragmentShader;
    GLuint Shader;
};

struct static_sprite_data
{
    ivec2 AtlasOffset;
    ivec2 SpriteSize;
};

struct orthocamera2d
{
    mat4 Matrix;
    vec2 Position;
    vec2 Viewport;
    
    real32 Zoom;
};

struct glyph
{
    vec2 Offset;
    vec2 Advance;
    ivec2 GlyphSize;
    ivec2 uv;
};

struct fontdata
{
    FT_Library FontLib;
    FT_Face FontFace;
    int32 FontSize;
    int32 AtlasPadding;
};

struct interfacetext
{
    material Color;
    real32 FontSize;
};

struct glrenderdata
{
    uint32 TransformCounter;
    renderertransform *RendererTransforms;

    uint32 UITransformCounter;
    renderertransform *UITransforms;
    
    GLuint RendererTransformsSBOID;
    GLuint ScreenSizeID;
    GLuint OrthographicMatrixID;

    GLuint LM_FontAtlasID;
    int32 FontHeight;
    glyph Glyphs[128];
    
    shader Shaders[1];
    
    static_sprite_data StaticSprites[20];
    texture2d Textures[2];

    orthocamera2d GameCamera;
    orthocamera2d UICamera;

    vec4 ClearColor;
};

internal vec4 
HexToRGBA(int64 hex) 
{
    vec4 Result = {};

    uint8 R = (hex>>24) & 0x000000FF;
    uint8 G = (hex>>16) & 0x000000FF;
    uint8 B = (hex>>8) & 0x000000FF;
    uint8 A = (hex>>0) & 0x000000FF;

    Result = 
    {
        (real32)R / 255.0f,
        (real32)G / 255.0f,
        (real32)B / 255.0f,
        (real32)A / 255.0f,
    };

    return(Result);
}

///////////////////////////////////////////////////
// NOTE(Sleepster): Matrix stuffs
internal mat4
CreateOrthographicMatrix(vec4 Data)
{
    mat4 Result = {};
   
    Result.Elements[3][0] = -(Data.Right + Data.Left) / (Data.Right - Data.Left);
    Result.Elements[3][1] = -(Data.Top + Data.Bottom) / (Data.Top - Data.Bottom);
    Result.Elements[3][2] =  0.0f;
    Result.Elements[3][3] =  1.0f;
    
    Result.Elements[0][0] =  2.0f / (Data.Right - Data.Left);
    Result.Elements[1][1] =  2.0f / (Data.Top - Data.Bottom);
    Result.Elements[2][2] =  1.0f / (1.0f - 0.0f);
    
    return(Result);
}

internal mat4
RotateZ(real32 Angle)
{
    mat4 Result = {};
    
    Result.Elements[0][0] = (real32)cos(Angle);
    Result.Elements[1][0] = (real32)-sin(Angle);
    
    Result.Elements[0][1] = (real32)sin(Angle);
    Result.Elements[1][1] = (real32)cos(Angle);
    
    Result.Elements[2][2] = 1.0f;
    Result.Elements[3][3] = 1.0f;
    
    return(Result);
}

internal inline bool
operator==(material A, material B)
{
    return(A.Color == B.Color);
}

#endif //_SHIVER__RENDERER_H
