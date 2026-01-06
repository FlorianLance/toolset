
#version 430

layout(binding=0) uniform sampler2D Texture0;
layout( location = 0 ) out vec4 FragColor;

void main() {

    ivec2 pix = ivec2( gl_FragCoord.xy );
    FragColor = texelFetch(Texture0, pix, 0);
}
