#version 430 core


uniform float hSize = 0.5f;   // Half the width of the quad

in vec4 vFragColorVs[];
in mat4 mvp[];
out vec4 vFragColor;

// model
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


layout (points) in;
layout( triangle_strip, max_vertices = 14 ) out;

void main(){

    vFragColor = vFragColorVs[0];
    vec4 center = gl_in[0].gl_Position;
    vec4 p0 = projection* (center + vec4(hSize,hSize,hSize,1));     // 1.0,	1.0,	1.0,
    vec4 p1 = projection* (center + vec4(-hSize,hSize,hSize,1));    // 0.0f,	1.0,	1.0,
    vec4 p2 = projection* (center + vec4(hSize,hSize,-hSize,1));    // 1.0,	1.0,	0.0f,
    vec4 p3 = projection* (center + vec4(-hSize,hSize,-hSize,1));   // 0.0f,	1.0,	0.0f,
    vec4 p4 = projection* (center + vec4(hSize,-hSize,hSize,1));    // 1.0,	0.0f,	1.0,
    vec4 p5 = projection* (center + vec4(-hSize,-hSize,hSize,1));   // 0.0f,	0.0f,	1.0,
    vec4 p6 = projection* (center + vec4(-hSize,-hSize,-hSize,1));  // 0.0f,	0.0f,	0.0f,
    vec4 p7 = projection* (center + vec4(hSize,-hSize,-hSize,1));   // 1.0,	0.0f,	0.0f

    gl_Position = p3;
    EmitVertex();

    gl_Position = p2;
    EmitVertex();

    gl_Position = p6;
    EmitVertex();

    gl_Position = p7;
    EmitVertex();

    gl_Position = p4;
    EmitVertex();

    gl_Position = p2;
    EmitVertex();

    gl_Position = p0;
    EmitVertex();

    gl_Position = p3;
    EmitVertex();

    gl_Position = p1;
    EmitVertex();

    gl_Position = p6;
    EmitVertex();

    gl_Position = p5;
    EmitVertex();

    gl_Position = p4;
    EmitVertex();

    gl_Position = p1;
    EmitVertex();

    gl_Position = p0;
    EmitVertex();

    //EndPrimitive();
}

