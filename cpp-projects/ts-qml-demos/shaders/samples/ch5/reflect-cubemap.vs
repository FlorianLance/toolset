#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 ReflectDir; // direction of the relfected ray

uniform vec3 WorldCameraPosition;
uniform mat4 ModelMatrix;
uniform mat4 MVP;

void main(){

    // compute the reflected direction in world coords
    vec3 worldPos = vec3(ModelMatrix*vec4(VertexPosition,1.0));
    vec3 worldNorm = vec3(ModelMatrix*vec4(VertexNormal, 0.0));
    vec3 worldView = normalize(WorldCameraPosition - worldPos);
    ReflectDir = reflect(-worldView, worldNorm);
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
