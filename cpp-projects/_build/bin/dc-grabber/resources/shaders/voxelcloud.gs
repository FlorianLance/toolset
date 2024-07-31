#version 430 core

// layout
layout (points) in;
layout( triangle_strip, max_vertices = 4 ) out;

// uniforms
// # transforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
// # camera
uniform vec3 camera_position;
// # geometry
uniform float squareSize = 0.002f;

// varyings
// # in
in vec4 vFragColorVs[];
in mat4 mvp[];
in int discardVertex[];
// # out
out vec4 vFragColor;
out vec2 tex_coord;

void main(){
	
    vFragColor = vFragColorVs[0];

    // square
    if(discardVertex[0] != 1)
    {
        // p1---p2
        // |  c |
        // p4---p3

        vec4 center = gl_in[0].gl_Position;
        vec4 p1 = projection * (center + vec4(-squareSize,-squareSize,-squareSize,1));  // 0.0f,	0.0f,	0.0f,
        vec4 p2 = projection * (center + vec4(squareSize,-squareSize,-squareSize,1));   // 1.0,		0.0f,	0.0f
        vec4 p3 = projection * (center + vec4(squareSize,squareSize,-squareSize,1));    // 1.0,		1.0,	0.0f,
        vec4 p4 = projection * (center + vec4(-squareSize,squareSize,-squareSize,1));   // 0.0f,	1.0,	0.0f,

        gl_Position = p4;
        tex_coord = vec2(0,1);
        EmitVertex();

        gl_Position = p1;
        tex_coord = vec2(0,0);
        EmitVertex();

        gl_Position = p3;
        tex_coord = vec2(1,1);
        EmitVertex();

        gl_Position = p2;
        tex_coord = vec2(1,0);
        EmitVertex();
    }

    /**

    // cube
    if(cube && discardVertex[0] != 1)
    {
        vec4 p0 = projection* (center + vec4(squareSize,squareSize,squareSize,1));     // 1.0,	1.0,	1.0,
            vec4 p1 = projection* (center + vec4(-squareSize,squareSize,squareSize,1));    // 0.0f,	1.0,	1.0,
            vec4 p2 = projection* (center + vec4(squareSize,squareSize,-squareSize,1));    // 1.0,	1.0,	0.0f,
            vec4 p3 = projection* (center + vec4(-squareSize,squareSize,-squareSize,1));   // 0.0f,	1.0,	0.0f,
            vec4 p4 = projection* (center + vec4(squareSize,-squareSize,squareSize,1));    // 1.0,	0.0f,	1.0,
            vec4 p5 = projection* (center + vec4(-squareSize,-squareSize,squareSize,1));   // 0.0f,	0.0f,	1.0,
            vec4 p6 = projection* (center + vec4(-squareSize,-squareSize,-squareSize,1));  // 0.0f,	0.0f,	0.0f,
            vec4 p7 = projection* (center + vec4(squareSize,-squareSize,-squareSize,1));   // 1.0,	0.0f,	0.0f

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
    }
    */

    EndPrimitive();
}

