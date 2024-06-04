#version 430 core

// layout
layout ( location = 0 ) out vec4 FragColor;

// uniforms
// # color
uniform vec4 unicolor = vec4(1,0,0,1);
uniform bool enable_unicolor = true;
uniform vec4 Color;

void main()
{
    FragColor = enable_unicolor ? unicolor : Color;
}
