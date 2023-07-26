#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNorm;
out vec4 vFragColorVs;

// transforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// camera
uniform vec3 camera_position;

// cloud
uniform float size_pt = 10.f;

// color
uniform bool enable_unicolor = false;
uniform float factor_unicolor = 0.3;
uniform vec4 unicolor = vec4(1,0,0,1);

void main(){

    vec4 p = view * model * vec4(aPos, 1.0);
    gl_Position = projection*p;

	vFragColorVs = enable_unicolor ? mix(unicolor,vec4(aColor, 1.0), factor_unicolor) : vec4(aColor, 1.0);
	//vFragColorVs = vec4(aNorm, 1.0);
	//vFragColorVs = vec4(aColor.x, aColor.y, aNorm.x, 1.0);
	
	
    float l = sqrt(length(p.xyz-camera_position.xyz));
    gl_PointSize = size_pt/(l);
}
