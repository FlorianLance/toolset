#version 430 core

// layout
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextCoord;

// uniform
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// out
out vec4 normal;
out vec2 textCoord;

void main(){	
	gl_Position = projection*view * model * vec4(aPos, 1.0);
	normal    = vec4(aNormal.xyz, 1);
	textCoord = aTextCoord;
}