#version 430 core

layout( location = 0 ) out vec4 FragColor;
in vec4 vFragColorVs;

void main(){
    FragColor = vFragColorVs;
}
