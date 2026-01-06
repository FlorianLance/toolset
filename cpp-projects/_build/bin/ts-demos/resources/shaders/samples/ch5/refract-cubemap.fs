#version 430


layout(binding=0) uniform samplerCube CubeMapTex;

in vec3 ReflectDir; // direction of the reflected ray
in vec3 RefractDir;    // transmitted direction

struct MaterialInfo{
    float Eta;              // ratio of indices of reffraction
    float ReflectionFactor; // percentage of reflected light
};
uniform MaterialInfo Material;

layout( location = 0 ) out vec4 FragColor;


void main() {
    // access the map texture1D
    vec4 reflectColor = texture(CubeMapTex, ReflectDir);
    vec4 refractColor = texture(CubeMapTex, RefractDir);

    FragColor = mix(reflectColor, refractColor, Material.ReflectionFactor);
}
