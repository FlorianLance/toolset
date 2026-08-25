#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;


out vec3 ReflectDir; // reflected direction
out vec3 RefractDir; // Transmitted direction

struct MaterialInfo{
    float Eta;              // ratio of indices of reffraction
    float ReflectionFactor; // percentage of reflected light
};
uniform MaterialInfo Material;

uniform vec3 WorldCameraPosition;
uniform mat4 ModelMatrix;
uniform mat4 MVP;

void main(){

    // compute the reflected direction in world coords
    vec3 worldPos = vec3(ModelMatrix*vec4(VertexPosition,1.0));
    vec3 worldNorm = vec3(ModelMatrix*vec4(VertexNormal, 0.0));
    vec3 worldView = normalize(WorldCameraPosition - worldPos);

    ReflectDir = reflect(-worldView, worldNorm);
    RefractDir = refract(-worldView, worldNorm, Material.Eta);

    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
