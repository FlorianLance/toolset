#version 450 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec4 aColor; // the color variable has attribute position 1

// model
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// color
uniform vec4 unicolor;
uniform bool enable_unicolor = false;
out vec4 color;

void main(){

    gl_Position = projection*view * model * vec4(aPos, 1.0);
    color = enable_unicolor ? unicolor : aColor;
}

