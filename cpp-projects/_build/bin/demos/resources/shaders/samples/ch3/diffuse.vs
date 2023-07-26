

#version 440

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 LightIntensity;

uniform vec4 LightPosition; // Light position in eye coords.
uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ld;            // Diffuse light intensity

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

// model
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    vec3 tnorm = normalize( NormalMatrix * VertexNormal);
    vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition,1.0);
    vec3 s = normalize(vec3(LightPosition - eyeCoords));
    LightIntensity =  Ld * Kd * max( dot( s, tnorm ), 0.0 );
    gl_Position = MVP * vec4(VertexPosition,1.0);

    //LightIntensity =  vec3(1,0,0);//Ld * Kd * max( dot( s, tnorm ), 0.0 );
    // equivalent
//    gl_Position = projection * view * model * vec4(VertexPosition,1.0);
    //vec4 p = view * model * vec4(VertexPosition, 1.0);
//    gl_Position = view * model * projection*vec4(VertexPosition,1.0);
//    gl_Position = vec4(VertexPosition,1.0)*projection*view*model;
}
