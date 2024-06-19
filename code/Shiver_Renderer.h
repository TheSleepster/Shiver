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
sh_glDrawStaticSprite2D(sprites SpriteID, vec2 Position, glrenderdata *RenderData)
{
    spritedata SpriteData = sh_glGetSprite(SpriteID, RenderData);
    
    renderertransform Transform  = {};
    Transform.AtlasOffset = SpriteData.AtlasOffset;
    Transform.SpriteSize = SpriteData.SpriteSize;
    Transform.WorldPosition = Position;
    Transform.Size = {100.0f, 100.0f};
    RenderData->RendererTransforms[RenderData->TransformCounter++] = Transform;
}

#endif //_SHIVER__RENDERER_H
