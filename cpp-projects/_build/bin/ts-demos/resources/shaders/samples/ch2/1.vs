#version 440 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VectexTexCoord;

out vec3 TexCoord;

void main(){
    TexCoord = VectexTexCoord;
    gl_Position = vec4(VertexPosition, 1.0);
}
