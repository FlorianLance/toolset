#version 400

layout (location = 0 ) in vec3 VertexPosition;

// model
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
//    gl_Position = projection*view * model * vec4(VertexPosition, 1.0);
//    gl_Position = projection * view * model * vec4(VertexPosition, 1.0);
    vec4 p = view * model * vec4(VertexPosition, 1.0);
    gl_Position = projection*p;
}
