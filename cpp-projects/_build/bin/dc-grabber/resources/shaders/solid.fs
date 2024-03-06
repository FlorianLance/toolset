#version 400

uniform vec4 Color;

layout ( location = 0 ) out vec4 FragColor;

// color
uniform vec4 unicolor = vec4(1,0,0,1);
uniform bool enable_unicolor = true;


void main()
{
    FragColor = unicolor;//enable_unicolor ? unicolor : Color;
}
