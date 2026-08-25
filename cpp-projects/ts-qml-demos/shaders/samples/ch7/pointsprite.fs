#version 440

in vec2 TexCoord;

layout (binding=0) uniform sampler2D SpriteTex;
//uniform sampler2D SpriteTex;

layout( location = 0 ) out vec4 FragColor;

void main()
{
    FragColor = texture(SpriteTex, TexCoord);
}
