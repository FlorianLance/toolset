#version 430

in vec3 ReflectDir; // direction of the relfected ray

layout(binding=0) uniform samplerCube CubeMapTex;

uniform float ReflectFactor; // Amount of reflection
uniform vec4 MaterialColor; // Color of the object's "Tint"

layout( location = 0 ) out vec4 FragColor;


void main() {
    // access the map texture1D
    vec4 CubeMapColor = texture(CubeMapTex, ReflectDir);
    FragColor = mix(MaterialColor, CubeMapColor, ReflectFactor);
}
