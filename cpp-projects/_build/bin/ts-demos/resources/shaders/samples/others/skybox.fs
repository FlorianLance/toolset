#version 430

layout(binding=0) uniform samplerCube SkyBoxTex;

in vec3 Position;

layout( location = 0 ) out vec4 FragColor;

void main() {
    vec3 texColor = texture(SkyBoxTex, normalize(Position)).rgb;
    texColor = pow( texColor, vec3(1.0/2.2));
    FragColor = vec4(texColor,1);
}
