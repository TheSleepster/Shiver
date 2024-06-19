/* date = June 18th 2024 1:01 pm */

#ifndef _SHIVER__RENDERER_H
#define _SHIVER__RENDERER_H

#include "../data/shader/Shiver_SharedShaderHeader.h"
#include "util/MemoryArena.h"
#include "util/Math.h"

// DEFINES
#define MAX_TRANSFORMS 10000
#define ArraySize(Array, Type) (sizeof(Array) / sizeof(Type))
#define SIMRATE ((1.0f/60.0f)*1000)

#include "OpenGL/GLL.h"
#include "OpenGL/glcorearb.h"
#include "OpenGL/glext.h"
#include "OpenGL/wglext.h"

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

struct orthocamera2d
{
    mat4 Matrix;
    vec2 Position;
    vec2 Viewport;
    
    real32 Zoom;
};

enum CameraState
{
    CAMERA_GAME,
    CAMERA_UI,
    CAMERA_EDITOR
};

struct glrenderdata
{
    uint32 TransformCounter;
    renderertransform RendererTransforms[MAX_TRANSFORMS];
    
    GLuint RendererTransformsSBOID;
    GLuint ScreenSizeID;
    GLuint OrthographicMatrixID;
    
    shader Shaders[1];
    
    spritedata Sprites[20];
    texture2d Textures[31];
    
    orthocamera2d Cameras[3];
};

internal void 
sh_glCreateStaticSprite2D(ivec2 AtlasOffset, ivec2 SpriteSize, sprites SpriteID, glrenderdata *RenderData)
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
sh_glDrawStaticSprite2D(sprites SpriteID, vec2 Position, ivec2 Size, glrenderdata *RenderData)
{
    spritedata SpriteData = sh_glGetSprite(SpriteID, RenderData);
    
    renderertransform Transform  = {};
    Transform.AtlasOffset = SpriteData.AtlasOffset;
    Transform.SpriteSize = SpriteData.SpriteSize;
    Transform.Size = v2Cast(Size);
    Transform.WorldPosition = Position - (Transform.Size / 2.);
    RenderData->RendererTransforms[RenderData->TransformCounter++] = Transform;
}

internal mat4
CreateOrthographicMatrix(vec4 Data)
{
    mat4 Result = {};
    
    Result.Elements[3][0] = -(Data.Right + Data.Left) / (Data.Right - Data.Left);
    Result.Elements[3][1] =  (Data.Top + Data.Bottom) / (Data.Top - Data.Bottom);
    Result.Elements[3][2] =  0.0f;
    
    Result.Elements[0][0] = 2.0f / (Data.Right - Data.Left);
    Result.Elements[1][1] = 2.0f / (Data.Top - Data.Bottom);
    Result.Elements[2][2] = 1.0f / (1.0f - 0.0f);
    Result.Elements[3][3] = 1.0f;
    
    return(Result);
}

#endif //_SHIVER__RENDERER_H