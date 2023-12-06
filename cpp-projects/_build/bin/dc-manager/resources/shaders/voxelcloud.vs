#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec4 vFragColorVs;

// transforms
uniform mat4 model;
uniform mat4 view;

// color
uniform bool enable_unicolor = false;
uniform float factor_unicolor = 0.3;
uniform vec4 unicolor = vec4(1,0,0,1);

void main(){
    gl_Position = view * model * vec4(aPos, 1.0);
	vFragColorVs = enable_unicolor ? mix(unicolor,vec4(aColor, 1.0), factor_unicolor) : vec4(aColor, 1.0);	
}
