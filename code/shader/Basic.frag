
//Input
layout(location = 0) in vec2 TextureCoordsIn;
// Output
layout(location = 0) out vec4 FragColor;

//Binding
layout(binding = 0) uniform sampler2D TextureAtlas;

void main()
{
    vec4 TextureColor = texelFetch(TextureAtlas, ivec2(TextureCoordsIn), 0);
    if(TextureColor.a == 0)
    {
        discard;
    }
    FragColor = TextureColor;
}