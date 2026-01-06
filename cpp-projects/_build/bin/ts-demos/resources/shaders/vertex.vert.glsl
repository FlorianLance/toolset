


#version 450 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec4 aColor; // the color variable has attribute position 1

// model
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// camera
uniform vec3 camera_position;

// cloud
uniform float size_pt = 3.f;

// color
uniform vec4 unicolor;
uniform bool enable_unicolor = false;
out vec4 color;

void main(){

    vec4 p = view * model * vec4(aPos, 1.0);
    gl_Position = projection*p;
    color = enable_unicolor ? unicolor : aColor;
    float l = sqrt(length(p.xyz-camera_position.xyz));
    gl_PointSize = size_pt/(l);
}

