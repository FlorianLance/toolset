#version 430 core

// layout
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNorm;

// uniforms
// # transforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection; 
// # camera
uniform vec3 camera_position;
// # color
uniform bool enable_unicolor = false;
uniform float factor_unicolor = 0.3;
uniform vec4 unicolor = vec4(1,0,0,1);
// # geometry
uniform bool backFaceCulling = true;

// varyings
// # out
out int discardVertex;
out vec4 vFragColorVs;

void main(){

    vec4 wP = model * (vec4(aPos, 1.0));
    vec4 wN = (model * vec4(aNorm, 1.0));
    vec3 dd = normalize(wP.xyz-camera_position);
    float dotV = dot(dd, wN.xyz);
    if(!backFaceCulling || dotV < 0){
            gl_Position = view * model * vec4(aPos, 1.0);
            discardVertex = 0;
    }else{
            discardVertex = 1;
    }

    vFragColorVs = enable_unicolor ? mix(unicolor,vec4(aColor, 1.0), factor_unicolor) : vec4(aColor, 1.0);
}
