#version 430 core

// layout
layout( location = 0 ) out vec4 FragColor;

// uniforms
// # geometry
uniform float circleRadius = 0.25f;

// varyings
// # in
in vec4 vFragColor;
in vec2 tex_coord;

void main(){    
    
	// discard pixels outside cirle
    float distanceX = abs(tex_coord.x - 0.5);
    float distanceY = abs(tex_coord.y - 0.5);
    if(((distanceX * distanceX) + (distanceY * distanceY)) > circleRadius){
		discard;
	}
	
	FragColor = vFragColor;
}
