#version 430 core

// layout
layout (location = 0 ) in vec3 aPos;

// uniforms
// #transforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 p = view * model * vec4(aPos, 1.0);
    gl_Position = projection*p;
}
